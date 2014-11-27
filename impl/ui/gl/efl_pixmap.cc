// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


extern "C" {
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <dri2/dri2.h>
#include <stdint.h>
#include <libdrm/drm.h>
#include <tbm_bufmgr.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
}

#include "ui/gl/efl_pixmap.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/logging.h"
#include "ui/gl/egl_util.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace gfx {

// Helper classes implementation.
class NativeDisplayConnection {
 public:
  NativeDisplayConnection() {
    display_ = XOpenDisplay(0);
    if (!display_) {
      LOG(ERROR) << "Failed to make connection with X";
      exit(0);
    }
  }

  ~NativeDisplayConnection() { XCloseDisplay(display_); }

  Display* GetDisplay() { return display_; }

 private:
  Display* display_;
};

class EGLDisplayConnection {
 public:
  EGLDisplayConnection(Display* native_display) {
    if (native_display) {
      egl_display_ =
          eglGetDisplay(reinterpret_cast<EGLNativeDisplayType>(native_display));
    } else {
      LOG(ERROR) << "EGLDisplayConnection failed - Invalid native display.";
      return;
    }

    if (egl_display_ == EGL_NO_DISPLAY) {
      LOG(ERROR) << "EGLDisplayConnection failed - "
                 << ui::GetLastEGLErrorString();
      return;
    }

    EGLint major, minor;
    if (eglInitialize(egl_display_, &major, &minor) != EGL_TRUE) {
      Terminate();
      LOG(ERROR) << "EGLDisplayConnection failed - "
                 << ui::GetLastEGLErrorString();
    }
  }

  ~EGLDisplayConnection() { Terminate(); }

  EGLDisplay GetDisplay() { return egl_display_; }

 private:
  void Terminate() {
    if (egl_display_ == EGL_NO_DISPLAY)
      return;

    eglTerminate(egl_display_);
    egl_display_ = EGL_NO_DISPLAY;
  }

  EGLDisplay egl_display_;
};

class OffScreenRootWindow {
 public:
  OffScreenRootWindow(Display* native_display) {
    window_ = 0;

    if (!native_display) {
      LOG(ERROR) << "OffScreenRootWindow failed - Invalid native display.";
      return;
    }

    native_display_ = native_display;
    XSetWindowAttributes attributes;
    attributes.override_redirect = true;
    window_ = XCreateSimpleWindow(
        native_display, XDefaultRootWindow(native_display), -1, -1, 1, 1, 0,
        BlackPixel(native_display, 0), WhitePixel(native_display, 0));
    XChangeWindowAttributes(native_display, window_, CWOverrideRedirect,
                            &attributes);
    XMapWindow(native_display, window_);

    if (!window_)
      LOG(ERROR) << "OffScreenRootWindow failed to create window.";
  }

  ~OffScreenRootWindow() {
    if (!native_display_)
      return;

    if (window_) {
      XUnmapWindow(native_display_, window_);
      XDestroyWindow(native_display_, window_);
      window_ = 0;
    }
  }

  Window GetRootWindow() { return window_; }

 private:
  Window window_;
  Display* native_display_;
};

Window GetOffscreenRootWindow() {
  static OffScreenRootWindow offscreen_root_window(
      EflPixmap::GetNativeDisplay());
  return offscreen_root_window.GetRootWindow();
}

// Tizen buffer implementation.
struct TbmInfo {
  int fd;
  tbm_bufmgr bufmgr;

  TbmInfo() : fd(-1), bufmgr(0) {}
};

TbmInfo g_tbm_info;

class NativeBufferManager {
 public:
  NativeBufferManager() { Initialize(); }
  ~NativeBufferManager() { Finalize(); }

 private:
  bool Initialize() {
    int event_base = 0;
    int error_base = 0;
    int dri2_major = 0;
    int dri2_minor = 0;
    char* driver_name = 0;
    char* device_name = 0;
    drm_magic_t magic = 0;
    int fd = -1;

    Display* dpy = EflPixmap::GetNativeDisplay();
    int screen = XDefaultScreen(dpy);
    if (!DRI2QueryExtension(dpy, &event_base, &error_base)) {
      LOG(ERROR) << "'DRI2QueryExtension' failed to get dri2 extension.";
      goto error;
    }

    if (!DRI2QueryVersion(dpy, &dri2_major, &dri2_minor)) {
      LOG(ERROR) << "'DRI2QueryVersion' failed to get dri2 version.";
      goto error;
    }

    if (!DRI2Connect(dpy, RootWindow(dpy, screen), &driver_name,
                     &device_name)) {
      LOG(ERROR) << "'DRI2Connect' failed to get dri2 connect.";
      goto error;
    }

    fd = open(device_name, O_RDWR);
    if (fd < 0) {
      LOG(ERROR) << "'DRM] couldn't open : ";
      goto error;
    }

    if (drmGetMagic(fd, &magic)) {
      LOG(ERROR) << "'drmGetMagic' failed to get magic.";
      goto error;
    }

    if (!DRI2Authenticate(dpy, RootWindow(dpy, screen), (unsigned int)magic)) {
      LOG(ERROR) << "'DRI2Authenticate' failed to get authentication.";
      goto error;
    }

    g_tbm_info.fd = fd;

    setenv("BUFMGR_LOCK_TYPE", "always", 1);
    setenv("BUFMGR_MAP_CACHE", "true", 1);
    g_tbm_info.bufmgr = tbm_bufmgr_init(fd);

    free(driver_name);
    free(device_name);
    return true;

  error:
    free(driver_name);
    free(device_name);
    if (fd > -1)
      close(fd);
    return false;
  }

  void Finalize() {
    if (g_tbm_info.bufmgr)
      tbm_bufmgr_deinit(g_tbm_info.bufmgr);

    if (g_tbm_info.fd > -1) {
      close(g_tbm_info.fd);
      g_tbm_info.fd = -1;
    }
  }
};

NativeBufferManager s_native_buffer_manager;

class NativeBuffer {
 public:
  NativeBuffer();
  ~NativeBuffer();

  void CreateDrawable(unsigned long pixmap_id);
  void DestroyDrawable(unsigned long pixmap_id);
  void* Lock(unsigned long pixmap_id, int count);
  bool Unlock();
  int GetStride(unsigned long pixmap_id);

 private:
  int pixmap_;
  DRI2Buffer* buffer_;
  tbm_bo buffer_object_;
};

NativeBuffer::NativeBuffer() : buffer_(NULL),
                               buffer_object_(NULL) {
}

NativeBuffer::~NativeBuffer() {
  if (buffer_) {
    XFree(buffer_);
    buffer_ = NULL;
  }
}

// TODO(prashant.n) Check drawable is needed on impl side or not.
void NativeBuffer::CreateDrawable(unsigned long pixmap_id) {
  DRI2CreateDrawable(EflPixmap::GetNativeDisplay(), pixmap_id);
}

void NativeBuffer::DestroyDrawable(unsigned long pixmap_id) {
  DRI2DestroyDrawable(EflPixmap::GetNativeDisplay(), pixmap_id);
}

void* NativeBuffer::Lock(unsigned long pixmap_id, int count) {
  tbm_bo_handle handle;
  unsigned int attach = DRI2BufferFrontLeft;
  int num = -1, width = 0, height = 0;

  DCHECK(pixmap_id);

  if (!buffer_) {
    DRI2Buffer* buffer = DRI2GetBuffers(EflPixmap::GetNativeDisplay(),
                                        pixmap_id,
                                        &width,
                                        &height,
                                        &attach,
                                        1,
                                        &num);
    if (!buffer || !buffer->name) {
      LOG(ERROR) << "'DRI2GetBuffers' failed to get dri2 buffers count :"
                 << count << "buf :" << buffer << "buf->name" << buffer->name;
      XFree(buffer);
      return NULL;
    }

    buffer_ = buffer;
  }

  if (!buffer_object_) {
    buffer_object_ = tbm_bo_import(g_tbm_info.bufmgr, buffer_->name);
    if (!buffer_object_) {
      LOG(ERROR) << "'tbm_bo_import' failed to import buffer";
      return NULL;
    }
  }

  handle = tbm_bo_map(buffer_object_,
                      TBM_DEVICE_CPU,
                      TBM_OPTION_READ | TBM_OPTION_WRITE);

  if (!handle.ptr) {
    LOG(ERROR) << "'tbm_bo_map' failed to map tbm buffer for GPU";
    return NULL;
  }

  return handle.ptr;
}

bool NativeBuffer::Unlock() {
  if (!buffer_object_)
    return false;

  tbm_bo_unmap(buffer_object_);
  tbm_bo_unref(buffer_object_);
  buffer_object_ = NULL;
  return true;
}

int NativeBuffer::GetStride(unsigned long pixmap_id) {
  DCHECK(buffer_);
  return buffer_->pitch;
}

// EflPixmapBase implemetation.
int EflPixmapBase::s_pixmap_surface_count = 0;

EflPixmapBase::EflPixmapBase(UsageType usage_type)
    : usage_type_(usage_type),
      pixmap_id_(0),
      native_buffer_(NULL) {
  native_buffer_ = new NativeBuffer;
}

EflPixmapBase::~EflPixmapBase() {
  DCHECK(native_buffer_);
  delete native_buffer_;
  native_buffer_ = NULL;
  pixmap_id_ = 0;
}

void* EflPixmapBase::Map() {
  DCHECK(usage_type_ == BUFFER);
  return native_buffer_->Lock(pixmap_id_,
                              EflPixmapBase::s_pixmap_surface_count);
}

bool EflPixmapBase::UnMap() {
  DCHECK(usage_type_ == BUFFER);
  return native_buffer_->Unlock();
}

int EflPixmapBase::GetStride() {
  DCHECK(usage_type_ == BUFFER);
  return native_buffer_->GetStride(pixmap_id_);
}


// EflPixmap implemetation.
// static
scoped_refptr<EflPixmap> EflPixmap::Create(UsageType usage_type,
                                           const gfx::Size& size) {
    EflPixmap* efl_pixmap = new EflPixmap(usage_type, size);

  if (!efl_pixmap->Initialize()) {
    LOG(ERROR) << "EflPixmap creation failed.";

    delete efl_pixmap;
    efl_pixmap = NULL;
  }

  return make_scoped_refptr(efl_pixmap);
}

EflPixmap::EflPixmap(UsageType usage_type, const gfx::Size& size)
    : EflPixmapBase(usage_type),
      size_(size),
      surface_(EGL_NO_SURFACE),
      used_(false) {
  EflPixmapBase::s_pixmap_surface_count++;
}

EflPixmap::~EflPixmap() {
  DCHECK(GetDisplay());

  if (surface_ != EGL_NO_SURFACE) {
    DestroySurface(surface_);
    surface_ = EGL_NO_SURFACE;
  }

  if (pixmap_id_) {
    native_buffer_->DestroyDrawable(pixmap_id_);
    FreePixmap(pixmap_id_);
    pixmap_id_ = 0;
  }

  delete native_buffer_;
  native_buffer_ = NULL;

  EflPixmapBase::s_pixmap_surface_count--;
}

// static
const char* EflPixmap::GetEglExtensions() {
  return eglQueryString(GetNativeDisplay(), EGL_EXTENSIONS);
}

// static
Display* EflPixmap::GetNativeDisplay() {
  static NativeDisplayConnection native_display_connection;
  return native_display_connection.GetDisplay();
}

// static
EGLDisplay EflPixmap::GetDisplay() {
  static EGLDisplayConnection egl_display_connection(GetNativeDisplay());
  return egl_display_connection.GetDisplay();
}

// static
EGLConfig EflPixmap::GetConfig() {
  static EGLConfig config = EflPixmap::ChooseConfig();
  return config;
}

bool EflPixmap::Initialize() {
  if (size_.IsEmpty() || !GetDisplay() || !GetConfig())
    return false;

  pixmap_id_ = CreatePixmap(size_);
  if (!pixmap_id_)
    return false;

  native_buffer_->CreateDrawable(pixmap_id_);

  if (usage_type_ == SURFACE) {
    surface_ = CreateSurface(pixmap_id_);

    if (surface_ == EGL_NO_SURFACE)
      return false;
  }

  return true;
}

// static
EGLConfig EflPixmap::ChooseConfig() {
  EGLint config_attributes_list[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT | EGL_PIXMAP_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
#if defined(OS_TIZEN_TV)
    EGL_SAMPLES, 4,
#else
    EGL_SAMPLES, 2,
#endif
    EGL_NONE
  };

  EGLConfig config = NULL;
  EGLint config_count = 0;
  if ((eglChooseConfig(GetDisplay(),
                       config_attributes_list,
                       &config,
                       1,
                       &config_count) != EGL_TRUE) && config_count) {
    LOG(ERROR) << "eglChooseConfig failed - " << ui::GetLastEGLErrorString();
    return NULL;
  }

  return config;
}

EGLSurface EflPixmap::CreateSurface(unsigned long pixmap_id) {
  DCHECK(GetDisplay());
  DCHECK(GetConfig());
  DCHECK(pixmap_id);

  EGLSurface surface =
      eglCreatePixmapSurface(GetDisplay(), GetConfig(), pixmap_id, 0);

  if (surface == EGL_NO_SURFACE) {
    LOG(ERROR) << "eglCreatePixmapSurface failed - "
               << ui::GetLastEGLErrorString();
  } else {
    eglSurfaceAttrib(GetDisplay(),
                     surface,
                     EGL_SWAP_BEHAVIOR,
                     EGL_BUFFER_PRESERVED);
  }

  return surface;
}

void EflPixmap::DestroySurface(EGLSurface surface) {
  DCHECK(GetDisplay());
  DCHECK(surface);

  if (eglMakeCurrent(GetDisplay(),
                     EGL_NO_SURFACE,
                     EGL_NO_SURFACE,
                     EGL_NO_CONTEXT) != EGL_TRUE) {
    LOG(ERROR) << "eglMakeCurrent failed - " << ui::GetLastEGLErrorString();
  }

  eglDestroySurface(GetDisplay(), surface);
}

unsigned long EflPixmap::CreatePixmap(const gfx::Size& size) {
  DCHECK(!size.IsEmpty());

  // FIXME(prashant.n): Get depth (value 24) from xvisuals or geometry.
  unsigned long pixmap_id = XCreatePixmap(GetNativeDisplay(),
                                          GetOffscreenRootWindow(),
                                          size.width(),
                                          size.height(),
                                          24);
  XFlush(GetNativeDisplay());
  return pixmap_id;
}

void EflPixmap::FreePixmap(unsigned long pixmap_id) {
  DCHECK(pixmap_id);
  XFreePixmap(GetNativeDisplay(), pixmap_id);
}

// EflPixmapImpl implementation.
EflPixmapImpl::EflPixmapImpl(UsageType usage_type, unsigned long pixmap_id)
    : EflPixmapBase(usage_type) {
  DCHECK(usage_type_ == BUFFER);
  DCHECK(pixmap_id);
  pixmap_id_ = pixmap_id;
}

EflPixmapImpl::~EflPixmapImpl() {
  pixmap_id_ = 0;
}

}  // namespace gfx
