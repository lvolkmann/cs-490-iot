import urllib.request as req
import json
import time

from Adafruit_IO import Client, Feed, RequestError

thingspeak_api = "https://api.thingspeak.com/channels/984999/feeds.json?api_key=GRB9FYFQVCLMYHGJ&results=2"


# conn = req.urlopen(thingspeak_api)

# response = conn.read()
# print(response)
# data = json.loads(response)
# print(data["feeds"][-1]['field1'])


class ThingSpeaker():

    def __init__(self, api, field_count=1):
        self.api = api
        self.conn = req.urlopen(api)
        self.field_count = field_count
        self.channel_data = json.loads(self.conn.read())["channel"]

    def get_response(self):
        self.conn = req.urlopen(self.api)
        response = self.conn.read()
        if response is not None:
            return response
        else:
            for _ in range(5):
                time.sleep(1000)
                response = self.conn.read()
                if response is not None:
                    return response
            print("ERROR")

    def print_response(self):
        print(json.loads(self.get_response()))

    def print_fields(self):
        response = self.get_response()
        data = json.loads(response)
        feeds = data["feeds"]
        for feed in feeds:
            for field_num in range(1, self.field_count + 1):
                try:
                    active_field = "field{}".format(field_num)
                    field_name = self.channel_data[active_field]
                    field_val = feed[active_field]
                    print(field_name + ": " + field_val)
                except:
                    pass

    def return_fields_dict(self):
        response = self.get_response()
        data = json.loads(response)
        feeds = data["feeds"]

        my_special_dict = dict()

        for feed in feeds:
            for field_num in range(1, self.field_count + 1):
                try:
                    active_field = "field{}".format(field_num)
                    field_name = self.channel_data[active_field]
                    field_val = feed[active_field]

                    my_special_dict[field_name] = field_val
                except:
                    pass

        return my_special_dict


class adaFruitTalker():

    def __init__(self, username, api_key, feed_names: list):
        self.aio = Client(username, api_key)
        self.feed_names = feed_names
        self.feeds_dict = self.inialize_feeds()

    def inialize_feeds(self):

        feeds_dict = dict()

        for feed_name in self.feed_names:
            feeds_dict[feed_name] = self.aio.feeds(feed_name)

        return feeds_dict

    def write_feeds(self, thing_speak_to_ada_dict: dict, thing_speak_values: dict):

        for key, value in thing_speak_values.items():

            if value is None:
                continue

            ada_feed_name = thing_speak_to_ada_dict[key]
            ada_feed = self.feeds_dict[ada_feed_name]

            self.aio.send(ada_feed.key, value)


# speaker = ThingSpeaker(thingspeak_api, 6)

# speaker.print_fields()

adafruit_key = "aio_bsJm15T83XKZLCoszCVzKStCpjaG"
adafruit_username = "aaz00966"

# aio = Client(adafruit_username, adafruit_key)
#
#
#
# feed = 'temp'
# # feed_obj = Feed(name=feed)
# # temp_feed = aio.create_feed(feed_obj)
# temp_feed = aio.feeds(feed)
#
#
# aio.send_data(temp_feed.key, -5)

thingspeak_to_ada_dict = {
    "Barometer": "barometer",
    "Heartbeat": "bpm",
    "Dust": "dust",
    "Humidity": "humidity",
    "Light": "light",
    "Temperature": "temp"
}

ada_feed_names = [
    "barometer",
    "bpm",
    "dust",
    "humidity",
    "light",
    "temp"
]

speaker = ThingSpeaker(thingspeak_api, 6)
talker = adaFruitTalker(adafruit_username, adafruit_key, ada_feed_names)

while True:
    print(speaker.return_fields_dict())
    talker.write_feeds(thingspeak_to_ada_dict, speaker.return_fields_dict())
    time.sleep(1)
