"use strict";

const assert = require("assert");
const path = require("path");

function makeElement(value, checked) {
    return {
        value: value || "",
        checked: Boolean(checked),
        dataset: {},
        textContent: "",
        addEventListener: function() {}
    };
}

const elements = {
    "mock-time": makeElement("09:41"),
    "mock-time-synced": makeElement("", true),
    "mock-temperature": makeElement("26"),
    "mock-weather-available": makeElement("", true),
    "mock-wifi": makeElement("connected"),
    "mock-bluetooth": makeElement("connected"),
    "mock-apply": makeElement(),
    "mock-reset": makeElement(),
    "mock-start-demo": makeElement(),
    "mock-stop-demo": makeElement(),
    "mock-status": makeElement()
};
const bridgeCalls = [];
const parentMessages = [];
let messageHandler = null;
let nextTimer = 1;
const rootClasses = new Set();
const parentWindow = {
    postMessage: function(message, origin) {
        parentMessages.push({message: message, origin: origin});
    }
};

global.document = {
    documentElement: {
        dataset: {},
        classList: {
            add: function(name) {
                rootClasses.add(name);
            },
            contains: function(name) {
                return rootClasses.has(name);
            }
        }
    },
    getElementById: function(id) {
        return elements[id];
    },
    querySelectorAll: function() {
        return [];
    }
};
global.window = {
    parent: parentWindow,
    location: {origin: "http://example.test"},
    setTimeout: function() {
        return nextTimer++;
    },
    clearTimeout: function() {},
    addEventListener: function(type, handler) {
        if(type === "message") {
            messageHandler = handler;
        }
    }
};
global.Module = {
    _app_ui_mock_set_time: function() {
        bridgeCalls.push(["time"].concat(Array.from(arguments)));
        return 1;
    },
    _app_ui_mock_set_weather: function() {
        bridgeCalls.push(["weather"].concat(Array.from(arguments)));
        return 1;
    },
    _app_ui_mock_set_wifi: function() {
        bridgeCalls.push(["wifi"].concat(Array.from(arguments)));
        return 1;
    },
    _app_ui_mock_set_bluetooth: function() {
        bridgeCalls.push(["bluetooth"].concat(Array.from(arguments)));
        return 1;
    }
};

require(path.resolve(process.argv[2]));
assert.deepStrictEqual(
    window.lvglMock.presets,
    ["offline", "connecting", "online", "fully-connected"]);

Module.onRuntimeInitialized();
assert.strictEqual(
    document.documentElement.dataset.lvglPreviewLayout, "device");
assert.strictEqual(
    document.documentElement.classList.contains("lvgl-embedded"), true);
assert.strictEqual(parentMessages[0].message.action, "ready");
assert.strictEqual(parentMessages[0].origin, "http://example.test");

bridgeCalls.length = 0;
messageHandler({
    origin: "http://example.test",
    source: parentWindow,
    data: {
        channel: "zlyhub.lvgl.mock.v1",
        action: "apply-preset",
        name: "online"
    }
});
assert.deepStrictEqual(
    bridgeCalls.map(function(call) { return call[0]; }),
    ["time", "weather", "wifi", "bluetooth"]);
assert.strictEqual(
    parentMessages[parentMessages.length - 1].message.action, "state");
assert.strictEqual(
    parentMessages[parentMessages.length - 1].message.status.wifi,
    "connected");

window.lvglMock.presets.forEach(function(preset) {
    bridgeCalls.length = 0;
    window.lvglMock.applyPreset(preset);
    assert.deepStrictEqual(
        bridgeCalls.map(function(call) { return call[0]; }),
        ["time", "weather", "wifi", "bluetooth"]);
});

bridgeCalls.length = 0;
window.lvglMock.setStatus({
    time: { hour: 23, minute: 59, synced: true },
    weather: { temperatureC: -20, available: true },
    wifi: "connected",
    bluetooth: "advertising"
});
assert.deepStrictEqual(bridgeCalls[0], ["time", 23, 59, 1]);
assert.deepStrictEqual(bridgeCalls[1], ["weather", -20, 1]);
assert.deepStrictEqual(bridgeCalls[2], ["wifi", 2]);
assert.deepStrictEqual(bridgeCalls[3], ["bluetooth", 1]);

assert.throws(function() {
    window.lvglMock.setStatus({time: {hour: 24}});
}, /time\.hour/);
assert.throws(function() {
    window.lvglMock.applyPreset("missing");
}, /Unknown preset/);
