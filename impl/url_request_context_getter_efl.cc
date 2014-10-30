// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "url_request_context_getter_efl.h"

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/threading/worker_pool.h"
#include "base/threading/sequenced_worker_pool.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/cookie_store_factory.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/browser/net/sqlite_persistent_cookie_store.h"
#include "net/base/cache_type.h"
#include "net/cert/cert_verifier.h"
#include "net/dns/host_resolver.h"
#include "net/dns/mapped_host_resolver.h"
#include "net/http/http_auth_handler_factory.h"
#include "net/http/http_cache.h"
#include "net/http/http_server_properties_impl.h"
#include "net/proxy/proxy_service.h"
#include "net/ssl/channel_id_service.h"
#include "net/ssl/default_channel_id_store.h"
#include "net/ssl/ssl_config_service_defaults.h"
#include "net/url_request/data_protocol_handler.h"
#include "net/url_request/file_protocol_handler.h"
#include "net/url_request/static_http_user_agent_settings.h"
#include "net/url_request/url_request_context_storage.h"
#include "net/url_request/url_request_intercepting_job_factory.h"
#include "net/url_request/url_request_job_factory_impl.h"
#include "network_delegate_efl.h"
#include "eweb_context.h"


namespace content {

namespace {

void InstallProtocolHandlers(net::URLRequestJobFactoryImpl* job_factory,
                             ProtocolHandlerMap* protocol_handlers) {
  for (ProtocolHandlerMap::iterator it =
           protocol_handlers->begin();
       it != protocol_handlers->end();
       ++it) {
    bool set_protocol = job_factory->SetProtocolHandler(
        it->first, it->second.release());
    DCHECK(set_protocol);
  }
  protocol_handlers->clear();
}

}  // namespace

URLRequestContextGetterEfl::URLRequestContextGetterEfl(
    EWebContext& web_context,
    bool ignore_certificate_errors,
    const base::FilePath& base_path,
    const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& file_task_runner,
    ProtocolHandlerMap* protocol_handlers,
    URLRequestInterceptorScopedVector request_interceptors,
    net::NetLog* net_log)
    : web_context_(web_context),
      ignore_certificate_errors_(ignore_certificate_errors),
      base_path_(base_path),
      io_task_runner_(io_task_runner),
      file_task_runner_(file_task_runner),
      request_interceptors_(request_interceptors.Pass()),
      net_log_(net_log) {
  // Must first be created on the UI thread.
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (protocol_handlers)
    std::swap(protocol_handlers_, *protocol_handlers);

  proxy_config_service_.reset(
      net::ProxyService::CreateSystemProxyConfigService(
          io_task_runner, file_task_runner));
}

URLRequestContextGetterEfl::~URLRequestContextGetterEfl() {
}

net::URLRequestContext* URLRequestContextGetterEfl::GetURLRequestContext() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (!url_request_context_) {
    const CommandLine& command_line = *CommandLine::ForCurrentProcess();

    url_request_context_.reset(new net::URLRequestContext());
    if (net_log_) {
      url_request_context_->set_net_log(net_log_);
    }
    network_delegate_.reset(new net::NetworkDelegateEfl(*(web_context_.cookieManager())));

    url_request_context_->set_network_delegate(network_delegate_.get());
    storage_.reset(
        new net::URLRequestContextStorage(url_request_context_.get()));
    storage_->set_cookie_store(
        CreateCookieStore(CookieStoreConfig()));
    storage_->set_channel_id_service(new net::ChannelIDService(
        new net::DefaultChannelIDStore(NULL),
        base::WorkerPool::GetTaskRunner(true)));
    storage_->set_http_user_agent_settings(
        new net::StaticHttpUserAgentSettings("en-us,en", std::string()));

    scoped_ptr<net::HostResolver> host_resolver(
        net::HostResolver::CreateDefaultResolver(
            url_request_context_->net_log()));

    storage_->set_cert_verifier(net::CertVerifier::CreateDefault());
    storage_->set_transport_security_state(new net::TransportSecurityState);

    // TODO(jam): use v8 if possible, look at chrome code.
    storage_->set_proxy_service(
        net::ProxyService::CreateUsingSystemProxyResolver(
        proxy_config_service_.release(),
        0,
        url_request_context_->net_log()));

    storage_->set_ssl_config_service(new net::SSLConfigServiceDefaults);
    storage_->set_http_auth_handler_factory(
        net::HttpAuthHandlerFactory::CreateDefault(host_resolver.get()));
    storage_->set_http_server_properties(
        scoped_ptr<net::HttpServerProperties>(
            new net::HttpServerPropertiesImpl()));

    base::FilePath cache_path = base_path_.Append(FILE_PATH_LITERAL("Cache"));
    net::HttpCache::DefaultBackend* main_backend =
        new net::HttpCache::DefaultBackend(
            net::DISK_CACHE,
            net::CACHE_BACKEND_DEFAULT,
            cache_path,
            0,
            BrowserThread::GetMessageLoopProxyForThread(BrowserThread::CACHE)
                .get());

    net::HttpNetworkSession::Params network_session_params;
    network_session_params.cert_verifier =
        url_request_context_->cert_verifier();
    network_session_params.transport_security_state =
        url_request_context_->transport_security_state();
    network_session_params.channel_id_service =
        url_request_context_->channel_id_service();
    network_session_params.proxy_service =
        url_request_context_->proxy_service();
    network_session_params.ssl_config_service =
        url_request_context_->ssl_config_service();
    network_session_params.http_auth_handler_factory =
        url_request_context_->http_auth_handler_factory();
    network_session_params.network_delegate =
        network_delegate_.get();
    network_session_params.http_server_properties =
        url_request_context_->http_server_properties();
    network_session_params.net_log =
        url_request_context_->net_log();
    network_session_params.ignore_certificate_errors =
        ignore_certificate_errors_;
    if (command_line.HasSwitch(switches::kTestingFixedHttpPort)) {
      int value;
      base::StringToInt(command_line.GetSwitchValueASCII(
          switches::kTestingFixedHttpPort), &value);
      network_session_params.testing_fixed_http_port = value;
    }
    if (command_line.HasSwitch(switches::kTestingFixedHttpsPort)) {
      int value;
      base::StringToInt(command_line.GetSwitchValueASCII(
          switches::kTestingFixedHttpsPort), &value);
      network_session_params.testing_fixed_https_port = value;
    }
    if (command_line.HasSwitch(switches::kHostResolverRules)) {
      scoped_ptr<net::MappedHostResolver> mapped_host_resolver(
          new net::MappedHostResolver(host_resolver.Pass()));
      mapped_host_resolver->SetRulesFromString(
          command_line.GetSwitchValueASCII(switches::kHostResolverRules));
      host_resolver = mapped_host_resolver.Pass();
    }

    // Give |storage_| ownership at the end in case it's |mapped_host_resolver|.
    storage_->set_host_resolver(host_resolver.Pass());
    network_session_params.host_resolver =
        url_request_context_->host_resolver();

    net::HttpCache* main_cache = new net::HttpCache(
        network_session_params, main_backend);
    storage_->set_http_transaction_factory(main_cache);

    scoped_ptr<net::URLRequestJobFactoryImpl> job_factory(
        new net::URLRequestJobFactoryImpl());
    // Keep ProtocolHandlers added in sync with
    // ShellContentBrowserClient::IsHandledURL().
    InstallProtocolHandlers(job_factory.get(), &protocol_handlers_);
    bool set_protocol = job_factory->SetProtocolHandler(
        url::kDataScheme, new net::DataProtocolHandler);
    DCHECK(set_protocol);
    set_protocol = job_factory->SetProtocolHandler(
        url::kFileScheme,
        new net::FileProtocolHandler(
            BrowserThread::GetBlockingPool()->
                GetTaskRunnerWithShutdownBehavior(
                    base::SequencedWorkerPool::SKIP_ON_SHUTDOWN)));
    DCHECK(set_protocol);

    // Set up interceptors in the reverse order.
    scoped_ptr<net::URLRequestJobFactory> top_job_factory =
        job_factory.Pass();
    for (URLRequestInterceptorScopedVector::reverse_iterator i =
             request_interceptors_.rbegin();
         i != request_interceptors_.rend();
         ++i) {
      top_job_factory.reset(new net::URLRequestInterceptingJobFactory(
          top_job_factory.Pass(), make_scoped_ptr(*i)));
    }
    request_interceptors_.weak_clear();

    storage_->set_job_factory(top_job_factory.release());
  }

  return url_request_context_.get();
}

scoped_refptr<base::SingleThreadTaskRunner>
    URLRequestContextGetterEfl::GetNetworkTaskRunner() const {
  return BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO);
}

net::HostResolver* URLRequestContextGetterEfl::host_resolver() {
  return url_request_context_->host_resolver();
}

void URLRequestContextGetterEfl::SetCookieStoragePath(
    const base::FilePath& path,
    bool persist_session_cookies,
    bool file_storage)
{
  if (url_request_context_->cookie_store() &&
      (cookie_store_path_ == path)) {
    // The path has not changed so don't do anything.
    return;
  }

  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(&URLRequestContextGetterEfl::SetCookieStoragePath,
                 this, path, persist_session_cookies, file_storage));
    return;
  }

  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if(!file_storage) {
    CreateSQLitePersistentCookieStore(path, persist_session_cookies);
  } else {
    CreatePersistentCookieStore(path, persist_session_cookies);
  }
}

void URLRequestContextGetterEfl::CreateSQLitePersistentCookieStore(
    const base::FilePath& path,
    bool persist_session_cookies)
{
  using content::CookieStoreConfig;
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  scoped_refptr<SQLitePersistentCookieStore> persistent_store;

  if (path.empty())
    return;
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  if (base::DirectoryExists(path) ||
      base::CreateDirectory(path)) {
    const base::FilePath& cookie_path = path.AppendASCII("Cookies");
    persistent_store =
      new SQLitePersistentCookieStore(
          cookie_path,
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::IO),
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::DB),
          persist_session_cookies, NULL, NULL);
  } else {
    NOTREACHED() << "The cookie storage directory could not be created";
    return;
  }
  // Set the new cookie store that will be used for all new requests. The old
  // cookie store, if any, will be automatically flushed and closed when no
  // longer referenced.
  scoped_refptr<net::CookieMonster> cookie_monster =
      new net::CookieMonster(persistent_store.get(), NULL);
  storage_->set_cookie_store(cookie_monster.get());

  if (persistent_store.get() && persist_session_cookies)
    cookie_monster->SetPersistSessionCookies(true);

  cookie_store_path_ = path;
}

void URLRequestContextGetterEfl::CreatePersistentCookieStore(const base::FilePath& path,
    bool persist_session_cookies)
{
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  cookie_store_path_ = path;

  CookieStoreConfig config(
      cookie_store_path_.Append(FILE_PATH_LITERAL("Cookies")),
      CookieStoreConfig::PERSISTANT_SESSION_COOKIES,
      NULL,
      NULL);
  cookie_store_= CreateCookieStore(config);
  cookie_store_->GetCookieMonster()->SetPersistSessionCookies(persist_session_cookies);
  DCHECK(url_request_context_);
  url_request_context_->set_cookie_store(cookie_store_.get());
}

}; // namespace content
