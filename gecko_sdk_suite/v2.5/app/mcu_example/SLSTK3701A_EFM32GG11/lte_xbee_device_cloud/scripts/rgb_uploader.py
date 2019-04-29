# This example establishes a https connection, but doesn't provide the server certificate validation.
# Production code should implement certificate validation.
# -------------------------------------------------
from http.client import HTTPSConnection
from random import randint
import base64, time

# create HTTP basic authentication string, this consists of
# "username:password" base64 encoded

username = b"my_username" # enter your username
password = b"my_password" # enter your password

auth = base64.encodestring(b"%s:%s"%(username,password)).decode().strip()
headers = {"Authorization": "Basic %s" % auth,
           "Content-type": "application/json"}

# message to send to server
client = HTTPSConnection("remotemanager.digi.com")
client.connect()

while 1:
    message = """{"stream_id": "RGB",
    "stream_type": "STRING",
    "value": "%d,%d,%d"}""" % (randint(0,255), randint(0,255), randint(0,255))
    print(message)

    client.request("POST", "https://remotemanager.digi.com/ws/v1/streams/history", body=message, headers=headers)
    response = client.getresponse()

    print(response.status, response.reason)
    print(response.read().decode())
    print()
    time.sleep(10)
