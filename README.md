# mycofarm

Environmental control for a small indoor mushroom farm.

An Arduino reads temperature, humidity, CO2 and VOC levels, shows them on an OLED and drives a relay. A Raspberry Pi reads those figures over USB serial, logs them to [Adafruit IO](https://io.adafruit.com/) feeds, and switches the humidifier on and off through IFTTT webhooks to keep humidity in range.

## What's here

| Folder | Contents |
| --- | --- |
| `MycoFarmV1/` | `MycoFarmV1.ino`: the controller sketch (DHT22 temperature/humidity, CCS811 eCO2/TVOC, U8glib OLED, relay). `MycoFarmManager.py`: the Pi service that reads the sketch's serial output and talks to Adafruit IO and IFTTT |
| `TripleStirrer/` | A three-channel stirrer: a button and LED per channel, PWM motor speed |
| `DHTtester/` | Adafruit's DHT example sketch, for checking a sensor |
| `design/` | Fritzing wiring designs for the controller, the Martha tent setup and the stirrer |

## Running the Pi service

The manager needs Python 3 with `pyserial`, `adafruit-io`, `python-decouple` and `requests`, and the Arduino on `/dev/ttyUSB0`. Its settings come from a `.env` file next to the script (gitignored):

```
AIOUSER=<Adafruit IO username>
AIOKEY=<Adafruit IO key>
IFTTT_HUMI_ON=<IFTTT webhook URL that turns the humidifier on>
IFTTT_HUMI_OFF=<IFTTT webhook URL that turns it off>
```

It's meant to run as a systemd service (`sudo systemctl start mycofarmmanager`).

## Licence

MIT
