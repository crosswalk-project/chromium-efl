#include <WebKit2/WKBundle.h>
#include <WebKit2/WKBundleInitialize.h>
#include <WebKit2/WKBundlePage.h>
#include <WebKit2/WKURLRequest.h>

static WKBundleRef globalBundle;

static WKURLRequestRef willSendRequestForFrame(WKBundlePageRef page, WKBundleFrameRef frame, uint64_t resourceIdentifier, WKURLRequestRef request, WKURLResponseRef redirectResponse, const void *clientInfo)
{
}

static void didCreatePage(WKBundleRef bundle, WKBundlePageRef page, const void* clientInfo)
{
    WKBundlePageResourceLoadClient resourceLoadClient;
    memset(&resourceLoadClient, 0, sizeof(resourceLoadClient));
    resourceLoadClient.version = kWKBundlePageResourceLoadClientCurrentVersion;
    //resourceLoadClient.willSendRequestForFrame = willSendRequestForFrame;

    WKBundlePageSetResourceLoadClient(page, &resourceLoadClient);
}

static void didRecieveMessage(WKBundleRef bundle, WKStringRef name, WKTypeRef messageBody, const void* clientInfo)
{
    WKBundlePostSynchronousMessage(bundle, name, messageBody, 0);
}

void WKBundleInitialize(WKBundleRef bundle, WKTypeRef initializationUserData)
{
    globalBundle = bundle;

    WKBundleClient client = {
        kWKBundleClientCurrentVersion,
        0,
        didCreatePage,
        0, /* willDestroyPage */
        0, /* didInitializePageGroup */
        didRecieveMessage,
    };
    WKBundleSetClient(bundle, &client);
}
