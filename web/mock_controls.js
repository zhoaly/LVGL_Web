(function() {
    "use strict";

    var MOCK_CHANNEL = "zlyhub.lvgl.mock.v1";
    var embedded = window.parent !== window;
    var runtimeReady = false;
    var WIFI_VALUES = {
        disconnected: 0,
        connecting: 1,
        connected: 2
    };
    var BLUETOOTH_VALUES = {
        off: 0,
        advertising: 1,
        connected: 2
    };
    var DEFAULT_STATUS = {
        time: { hour: 0, minute: 0, synced: false },
        weather: { temperatureC: 0, available: false },
        wifi: "disconnected",
        bluetooth: "off"
    };
    var PRESETS = {
        offline: DEFAULT_STATUS,
        connecting: {
            time: { hour: 0, minute: 0, synced: false },
            weather: { temperatureC: 0, available: false },
            wifi: "connecting",
            bluetooth: "off"
        },
        online: {
            time: { hour: 9, minute: 41, synced: true },
            weather: { temperatureC: 26, available: true },
            wifi: "connected",
            bluetooth: "advertising"
        },
        "fully-connected": {
            time: { hour: 9, minute: 41, synced: true },
            weather: { temperatureC: 26, available: true },
            wifi: "connected",
            bluetooth: "connected"
        }
    };
    var currentStatus = cloneStatus(DEFAULT_STATUS);
    var demoTimers = [];
    var controls = null;

    if(embedded) {
        document.documentElement.dataset.lvglPreviewLayout = "device";
        document.documentElement.classList.add("lvgl-embedded");
    }

    function cloneStatus(status) {
        return {
            time: {
                hour: status.time.hour,
                minute: status.time.minute,
                synced: status.time.synced
            },
            weather: {
                temperatureC: status.weather.temperatureC,
                available: status.weather.available
            },
            wifi: status.wifi,
            bluetooth: status.bluetooth
        };
    }

    function integerInRange(value, min, max, name) {
        if(!Number.isInteger(value) || value < min || value > max) {
            throw new TypeError(name + " must be an integer from " +
                                min + " to " + max);
        }
        return value;
    }

    function booleanValue(value, name) {
        if(typeof value !== "boolean") {
            throw new TypeError(name + " must be a boolean");
        }
        return value;
    }

    function enumValue(value, values, name) {
        if(typeof value !== "string" ||
           !Object.prototype.hasOwnProperty.call(values, value)) {
            throw new TypeError(name + " has an unsupported value");
        }
        return value;
    }

    function mergeStatus(base, patch) {
        var next = cloneStatus(base);

        if(patch == null || typeof patch !== "object") {
            throw new TypeError("status must be an object");
        }
        if(patch.time != null) {
            if(typeof patch.time !== "object") {
                throw new TypeError("time must be an object");
            }
            if(patch.time.hour != null) {
                next.time.hour = integerInRange(
                    patch.time.hour, 0, 23, "time.hour");
            }
            if(patch.time.minute != null) {
                next.time.minute = integerInRange(
                    patch.time.minute, 0, 59, "time.minute");
            }
            if(patch.time.synced != null) {
                next.time.synced =
                    booleanValue(patch.time.synced, "time.synced");
            }
        }
        if(patch.weather != null) {
            if(typeof patch.weather !== "object") {
                throw new TypeError("weather must be an object");
            }
            if(patch.weather.temperatureC != null) {
                next.weather.temperatureC = integerInRange(
                    patch.weather.temperatureC,
                    -99,
                    99,
                    "weather.temperatureC");
            }
            if(patch.weather.available != null) {
                next.weather.available = booleanValue(
                    patch.weather.available,
                    "weather.available");
            }
        }
        if(patch.wifi != null) {
            next.wifi = enumValue(patch.wifi, WIFI_VALUES, "wifi");
        }
        if(patch.bluetooth != null) {
            next.bluetooth = enumValue(
                patch.bluetooth, BLUETOOTH_VALUES, "bluetooth");
        }
        return next;
    }

    function callBridge(name, args) {
        var bridge = Module["_" + name];

        if(typeof bridge !== "function") {
            throw new Error("Simulator bridge is not ready");
        }
        return bridge.apply(null, args) !== 0;
    }

    function postStatus(status) {
        var accepted = [
            callBridge("app_ui_mock_set_time", [
                status.time.hour,
                status.time.minute,
                status.time.synced ? 1 : 0
            ]),
            callBridge("app_ui_mock_set_weather", [
                status.weather.temperatureC,
                status.weather.available ? 1 : 0
            ]),
            callBridge("app_ui_mock_set_wifi", [
                WIFI_VALUES[status.wifi]
            ]),
            callBridge("app_ui_mock_set_bluetooth", [
                BLUETOOTH_VALUES[status.bluetooth]
            ])
        ];

        if(accepted.indexOf(false) !== -1) {
            throw new Error("UI event queue rejected mock data");
        }
    }

    function setPanelStatus(message) {
        if(controls != null) {
            controls.status.textContent = message;
        }
    }

    function postParent(action, detail) {
        var message;

        if(!embedded) {
            return;
        }
        message = Object.assign({
            channel: MOCK_CHANNEL,
            action: action
        }, detail || {});
        window.parent.postMessage(message, window.location.origin);
    }

    function sendReady() {
        if(!runtimeReady) {
            return;
        }
        postParent("ready", {
            presets: Object.keys(PRESETS),
            status: cloneStatus(currentStatus)
        });
    }

    function sendState() {
        if(runtimeReady) {
            postParent("state", {status: cloneStatus(currentStatus)});
        }
    }

    function sendError(error) {
        postParent("error", {
            message: error instanceof Error ? error.message : String(error)
        });
    }

    function syncControls(status) {
        if(controls == null) {
            return;
        }

        controls.time.value =
            String(status.time.hour).padStart(2, "0") + ":" +
            String(status.time.minute).padStart(2, "0");
        controls.timeSynced.checked = status.time.synced;
        controls.temperature.value = String(status.weather.temperatureC);
        controls.weatherAvailable.checked = status.weather.available;
        controls.wifi.value = status.wifi;
        controls.bluetooth.value = status.bluetooth;
    }

    function cancelDemo(updateMessage) {
        demoTimers.forEach(function(timer) {
            window.clearTimeout(timer);
        });
        demoTimers = [];
        if(updateMessage) {
            setPanelStatus("Demo stopped");
        }
    }

    function applyStatus(status, options) {
        var settings = options || {};

        if(settings.stopDemo !== false) {
            cancelDemo(false);
        }
        postStatus(status);
        currentStatus = cloneStatus(status);
        syncControls(currentStatus);
        setPanelStatus(settings.message || "Mock data applied");
        sendState();
        return cloneStatus(currentStatus);
    }

    function setStatus(patch) {
        return applyStatus(mergeStatus(currentStatus, patch), {
            message: "Mock data applied"
        });
    }

    function applyPreset(name) {
        if(!Object.prototype.hasOwnProperty.call(PRESETS, name)) {
            throw new TypeError("Unknown preset: " + name);
        }
        return applyStatus(cloneStatus(PRESETS[name]), {
            message: "Preset: " + name
        });
    }

    function startDemo() {
        var sequence = [
            { delay: 0, name: "offline" },
            { delay: 1500, name: "connecting" },
            { delay: 3500, name: "online" },
            { delay: 5500, name: "fully-connected" }
        ];

        cancelDemo(false);
        sequence.forEach(function(step, index) {
            demoTimers.push(window.setTimeout(function() {
                applyStatus(cloneStatus(PRESETS[step.name]), {
                    stopDemo: false,
                    message: index === sequence.length - 1 ?
                        "Demo complete" : "Demo: " + step.name
                });
                if(index === sequence.length - 1) {
                    demoTimers = [];
                }
            }, step.delay));
        });
        setPanelStatus("Demo started");
    }

    function stopDemo() {
        cancelDemo(true);
    }

    function reset() {
        return applyStatus(cloneStatus(DEFAULT_STATUS), {
            message: "Mock data reset"
        });
    }

    function statusFromControls() {
        var timeParts = controls.time.value.split(":");

        return mergeStatus(currentStatus, {
            time: {
                hour: Number(timeParts[0]),
                minute: Number(timeParts[1]),
                synced: controls.timeSynced.checked
            },
            weather: {
                temperatureC: Number(controls.temperature.value),
                available: controls.weatherAvailable.checked
            },
            wifi: controls.wifi.value,
            bluetooth: controls.bluetooth.value
        });
    }

    function reportControlError(error) {
        setPanelStatus(error instanceof Error ? error.message : String(error));
    }

    function setWifiScenario(scenario) {
        integerInRange(scenario, 0, 8, "wifi scenario");
        if(!callBridge("app_ui_mock_wifi_scenario", [scenario])) {
            throw new Error("Wi-Fi operation active; try again after completion");
        }
        setPanelStatus("Wi-Fi scenario: " + scenario);
    }

    function bindControls() {
        controls = {
            time: document.getElementById("mock-time"),
            timeSynced: document.getElementById("mock-time-synced"),
            temperature: document.getElementById("mock-temperature"),
            weatherAvailable:
                document.getElementById("mock-weather-available"),
            wifi: document.getElementById("mock-wifi"),
            bluetooth: document.getElementById("mock-bluetooth"),
            apply: document.getElementById("mock-apply"),
            reset: document.getElementById("mock-reset"),
            startDemo: document.getElementById("mock-start-demo"),
            stopDemo: document.getElementById("mock-stop-demo"),
            status: document.getElementById("mock-status")
        };

        var scenario = document.getElementById("mock-wifi-scenario");
        if(scenario) scenario.addEventListener("change", function() {
            try { setWifiScenario(Number(scenario.value)); }
            catch(error) { reportControlError(error); }
        });
        controls.apply.addEventListener("click", function() {
            try {
                applyStatus(statusFromControls(), {
                    message: "Mock data applied"
                });
            }
            catch(error) {
                reportControlError(error);
            }
        });
        controls.reset.addEventListener("click", function() {
            try {
                reset();
            }
            catch(error) {
                reportControlError(error);
            }
        });
        controls.startDemo.addEventListener("click", function() {
            try {
                startDemo();
            }
            catch(error) {
                reportControlError(error);
            }
        });
        controls.stopDemo.addEventListener("click", stopDemo);

        document.querySelectorAll("[data-mock-preset]").forEach(
            function(button) {
                button.addEventListener("click", function() {
                    try {
                        applyPreset(button.dataset.mockPreset);
                    }
                    catch(error) {
                        reportControlError(error);
                    }
                });
            });

        syncControls(currentStatus);
    }

    window.lvglMock = {
        setStatus: setStatus,
        setWifiScenario: setWifiScenario,
        applyPreset: applyPreset,
        startDemo: startDemo,
        stopDemo: stopDemo,
        reset: reset,
        presets: Object.keys(PRESETS)
    };

    window.addEventListener("message", function(event) {
        var message = event.data;

        if(!embedded ||
           event.origin !== window.location.origin ||
           event.source !== window.parent ||
           message == null ||
           message.channel !== MOCK_CHANNEL) {
            return;
        }

        try {
            switch(message.action) {
                case "probe":
                    sendReady();
                    break;
                case "wifi-scenario":
                    setWifiScenario(message.scenario);
                    break;
                case "set-status":
                    setStatus(message.status);
                    break;
                case "apply-preset":
                    applyPreset(message.name);
                    break;
                case "start-demo":
                    startDemo();
                    break;
                case "stop-demo":
                    stopDemo();
                    break;
                case "reset":
                    reset();
                    break;
                default:
                    throw new TypeError(
                        "Unsupported mock action: " + message.action);
            }
        }
        catch(error) {
            reportControlError(error);
            sendError(error);
        }
    });

    var previousRuntimeInitialized = Module.onRuntimeInitialized;
    Module.onRuntimeInitialized = function() {
        if(typeof previousRuntimeInitialized === "function") {
            previousRuntimeInitialized();
        }
        runtimeReady = true;
        bindControls();
        sendReady();
        window.setTimeout(startDemo, 100);
    };
}());
