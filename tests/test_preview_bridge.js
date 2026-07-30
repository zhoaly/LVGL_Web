"use strict";

const assert = require("assert");
const path = require("path");

const posted = [];
let messageHandler = null;
const parentWindow = {
    postMessage: function(message, origin) {
        posted.push({message, origin});
    }
};

global.document = {
    documentElement: {
        dataset: {
            lvglPreviewLayout: "device",
            lvglPreviewProtocol: "1"
        }
    }
};
global.window = {
    parent: parentWindow,
    location: {origin: "http://example.test"},
    addEventListener: function(type, handler) {
        if(type === "message") {
            messageHandler = handler;
        }
    }
};

require(path.resolve(process.argv[2]));

messageHandler({
    origin: "http://example.test",
    source: parentWindow,
    data: {
        channel: "zlyhub.lvgl.preview.v1",
        action: "probe"
    }
});
assert.strictEqual(posted.length, 0);

window.lvglPreviewBridge.markReady({
    width: 240,
    height: 320,
    mockData: true,
    encoder: true
});
assert.strictEqual(posted.length, 1);
assert.deepStrictEqual(posted[0], {
    message: {
        channel: "zlyhub.lvgl.preview.v1",
        action: "ready",
        protocol: 1,
        layout: "device",
        width: 240,
        height: 320,
        capabilities: {
            mockData: true,
            encoder: true
        }
    },
    origin: "http://example.test"
});

messageHandler({
    origin: "http://example.test",
    source: parentWindow,
    data: {
        channel: "zlyhub.lvgl.preview.v1",
        action: "probe"
    }
});
assert.strictEqual(posted.length, 2);

messageHandler({
    origin: "http://wrong-origin.test",
    source: parentWindow,
    data: {
        channel: "zlyhub.lvgl.preview.v1",
        action: "probe"
    }
});
assert.strictEqual(posted.length, 2);
