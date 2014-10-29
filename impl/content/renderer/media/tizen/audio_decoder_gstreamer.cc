// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/tizen/audio_decoder_gstreamer.h"

#include "base/basictypes.h"
#include "base/memory/shared_memory.h"
#include "base/posix/eintr_wrapper.h"
#include "base/process/process.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "common/render_messages_efl.h"
#include "media/base/audio_bus.h"
#include "media/base/limits.h"
#include "public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebAudioBus.h"
#include "media/base/tizen/webaudio_media_codec_info_tizen.h"

namespace content {
//This class is similar as AudioDecoderIO class of Android defined
//in src/content/renderer/media/android/audio_decoder_android.cc
class AudioDecoderIO {
 public:
  AudioDecoderIO(const char* data, size_t data_size);
  ~AudioDecoderIO();
  bool ShareEncodedToProcess(base::SharedMemoryHandle* handle);

  // Returns true if AudioDecoderIO was successfully created.
  bool IsValid() const;

  int read_fd() const { return read_fd_; }
  int write_fd() const { return write_fd_; }

 private:
  // Shared memory that will hold the encoded audio data.  This is
  // used by MediaCodec for decoding.
  base::SharedMemory encoded_shared_memory_;

  // A pipe used to communicate with MediaCodec.  MediaCodec owns
  // write_fd_ and writes to it.
  int read_fd_;
  int write_fd_;

  DISALLOW_COPY_AND_ASSIGN(AudioDecoderIO);
};

AudioDecoderIO::AudioDecoderIO(const char* data, size_t data_size)
    : read_fd_(-1),
      write_fd_(-1) {

  if (!data || !data_size || data_size > 0x80000000)
    return;

  // Create the shared memory and copy our data to it so that
  // MediaCodec can access it.
  if(!encoded_shared_memory_.CreateAndMapAnonymous(data_size)) {
    LOG(ERROR) << __FUNCTION__ << " Creation of shared memory failed";
    return;
  }

  if (!encoded_shared_memory_.memory())
    return;

  memcpy(encoded_shared_memory_.memory(), data, data_size);

  // Create a pipe for reading/writing the decoded PCM data
  int pipefd[2];

  if (pipe(pipefd)) {
    LOG(INFO) <<" Pipe is already created";
    return;
  }

  read_fd_ = pipefd[0];
  write_fd_ = pipefd[1];
}

AudioDecoderIO::~AudioDecoderIO() {
  // Close the read end of the pipe.  The write end should have been
  // closed by MediaCodec.
  if (read_fd_ >= 0 && close(read_fd_)) {
    LOG(WARNING) << "Cannot close read fd " << read_fd_
             << ": " << strerror(errno);
  }
}

bool AudioDecoderIO::IsValid() const {
  return read_fd_ >= 0 && write_fd_ >= 0 &&
      encoded_shared_memory_.memory();
}

bool AudioDecoderIO::ShareEncodedToProcess(base::SharedMemoryHandle* handle) {
  return encoded_shared_memory_.ShareToProcess(base::GetCurrentProcessHandle(),
                                               handle);
}

static float ConvertSampleToFloat(int16_t sample) {
  const float kMaxScale = 1.0f / std::numeric_limits<int16_t>::max();
  const float kMinScale = -1.0f / std::numeric_limits<int16_t>::min();

  return sample * (sample < 0 ? kMinScale : kMaxScale);
}

static void CopyPcmDataToBus(int input_fd,
                        blink::WebAudioBus* destination_bus,
                        size_t num_of_frames,
                        unsigned number_of_channels,
                        double file_sample_rate) {

  int16_t pipe_data[PIPE_BUF / sizeof(int16_t)];
  ssize_t nread;
  std::vector<int16_t> decoded_samples;

  while ((nread = HANDLE_EINTR(read(input_fd, pipe_data, sizeof(pipe_data)))) > 0) {
    size_t samples_in_pipe = nread / sizeof(int16_t);

    if (decoded_samples.size() + samples_in_pipe > decoded_samples.capacity()) {
      decoded_samples.reserve(std::max(samples_in_pipe,
                                       2 * decoded_samples.capacity()));
    }
    std::copy(pipe_data,
              pipe_data + samples_in_pipe,
              back_inserter(decoded_samples));
  }

  size_t number_of_samples = decoded_samples.size();
  size_t number_of_frames = decoded_samples.size() / number_of_channels;
  size_t decoded_frames = 0;

  destination_bus->initialize(number_of_channels,
                              number_of_frames,
                              file_sample_rate);

  for (size_t m = 0; m < number_of_samples; m += number_of_channels) {
    for (size_t k = 0; k < number_of_channels; ++k) {
      int16_t sample = decoded_samples[m + k];
      destination_bus->channelData(k)[decoded_frames] =
        ConvertSampleToFloat(sample);
    }
    ++decoded_frames;
  }

  if (decoded_frames < number_of_frames)
    destination_bus->resizeSmaller(decoded_frames);
}

static void BufferAndCopyPcmDataToBus(int input_fd,
                                      blink::WebAudioBus* destination_bus,
                                      unsigned number_of_channels,
                                      double file_sample_rate) {
  int16_t pipe_data[PIPE_BUF / sizeof(int16_t)];
  std::vector<int16_t> decoded_samples;
  ssize_t nread;

  while ((nread = HANDLE_EINTR(read(input_fd, pipe_data, sizeof(pipe_data)))) > 0) {
    size_t samples_in_pipe = nread / sizeof(int16_t);
    if (decoded_samples.size() + samples_in_pipe > decoded_samples.capacity()) {
      decoded_samples.reserve(std::max(samples_in_pipe,
                                       2 * decoded_samples.capacity()));
    }
    std::copy(pipe_data,
              pipe_data + samples_in_pipe,
              back_inserter(decoded_samples));
  }

  // Convert the samples and save them in the audio bus.
  size_t number_of_samples = decoded_samples.size();
  size_t number_of_frames = decoded_samples.size() / number_of_channels;
  size_t decoded_frames = 0;

  destination_bus->initialize(number_of_channels,
                              number_of_frames,
                              file_sample_rate);

  for (size_t m = 0; m < number_of_samples; m += number_of_channels) {
    for (size_t k = 0; k < number_of_channels; ++k) {
      int16_t sample = decoded_samples[m + k];
      destination_bus->channelData(k)[decoded_frames] =
          ConvertSampleToFloat(sample);
    }
    ++decoded_frames;
  }

  // number_of_frames is only an estimate.  Resize the buffer with the
  // actual number of received frames.
  if (decoded_frames < number_of_frames)
    destination_bus->resizeSmaller(decoded_frames);
}

// Decode in-memory audio file data.
bool DecodeAudioFileData(blink::WebAudioBus* destination_bus,
                              const char* data,
                              size_t data_size,
                              scoped_refptr<ThreadSafeSender> sender) {
  AudioDecoderIO audio_decoder(data, data_size);

  if (!audio_decoder.IsValid()) {
    LOG(ERROR) << "Invalid audio_decoder";
    return false;
  }

  base::SharedMemoryHandle encoded_data_handle;
  audio_decoder.ShareEncodedToProcess(&encoded_data_handle);
  base::FileDescriptor fd(audio_decoder.write_fd(), true);

  // Start Decoding in the browser which will read from
  // encoded_data_handle for our shared memory and write the decoded
  // PCM samples (16-bit integer) to our pipe.

  sender->Send(new EflViewHostMsg_GstWebAudioDecode(encoded_data_handle, fd, data_size));

  int input_fd = audio_decoder.read_fd();
  struct media::WebAudioMediaCodecInfoTizen info;

  ssize_t nread = HANDLE_EINTR(read(input_fd, &info, sizeof(info)));

  if (nread != sizeof(info)) {
    LOG(ERROR) << "Read Failed";
    return false;
  }

  unsigned number_of_channels = info.channel_count;
  double file_sample_rate = static_cast<double>(info.sample_rate);
  size_t number_of_frames = info.number_of_frames;

  // Sanity checks
  if (!number_of_channels ||
      number_of_channels > media::limits::kMaxChannels ||
      file_sample_rate < media::limits::kMinSampleRate ||
      file_sample_rate > media::limits::kMaxSampleRate) {
    return false;
  }

  if (number_of_frames > 0) {
    CopyPcmDataToBus(input_fd,
                     destination_bus,
                     number_of_frames,
                     number_of_channels,
                     file_sample_rate);
  } else {
    BufferAndCopyPcmDataToBus(input_fd,
                              destination_bus,
                              number_of_channels,
                              file_sample_rate);
  }

  return true;
}

}  // namespace content
