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


# speaker = ThingSpeaker(thingspeak_api, 6)

# speaker.print_fields()

adafruit_key = "aio_hJLW69gPHNeID6cCV9dSLdDQFMF0"
adafruit_username = "aaz00966"

aio = Client(adafruit_username, adafruit_key)



feed = 'test2'
# feed_obj = Feed(name=feed)
# temp_feed = aio.create_feed(feed_obj)
temp_feed = aio.feeds(feed)


aio.send_data(temp_feed.key, 9)
