(function() {
    "use strict";

    var CHANNEL = "zlyhub.lvgl.preview.v1";
    var embedded = window.parent !== window;
    var readyPayload = null;

    function postReady() {
        if(!embedded || readyPayload == null) {
            return;
        }
        window.parent.postMessage(
            Object.assign({
                channel: CHANNEL,
                action: "ready"
            }, readyPayload),
            window.location.origin
        );
    }

    function markReady(options) {
        var settings = options || {};

        readyPayload = {
            protocol: 1,
            layout:
                document.documentElement.dataset.lvglPreviewLayout ||
                "device",
            width: settings.width,
            height: settings.height,
            capabilities: {
                mockData: settings.mockData === true,
                encoder: settings.encoder === true
            }
        };
        postReady();
    }

    window.addEventListener("message", function(event) {
        var message = event.data;

        if(!embedded ||
           event.origin !== window.location.origin ||
           event.source !== window.parent ||
           message == null ||
           message.channel !== CHANNEL ||
           message.action !== "probe") {
            return;
        }
        postReady();
    });

    window.lvglPreviewBridge = {
        markReady: markReady
    };
}());
