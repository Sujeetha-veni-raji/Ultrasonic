import mysql.connector
import paho.mqtt.client as mqttClient
from threading import Thread
import json


class Mqtt:
    def __init__(self):
        self.json_data = {}
        self.db = mysql.connector.connect(
            host="localhost",
            user="root",
            password="",
            db="distance")
        mqttclient = mqttClient.Client("52244535475668454")
        mqttclient.on_connect = self.on_connect
        mqttclient.on_message = self.on_message
        #print(mqttclient.on_message)
        mqttclient.username_pw_set(username="",password="")
        mqttstatus = mqttclient.connect("broker.emqx.io", 1883,60)
        mqttclient.subscribe("vishal",0)
        mqttclient.loop_forever()


    def upload(self,msg):
        mqtt_msg = str(msg.payload).replace("b'", "").replace("'", "").replace("  ", "").replace("\\n", "").replace("\n", '')
        print(msg.payload)
        print(msg.topic)
        mqtt_msg = str(mqtt_msg).replace("\\","")
        mqtt_msg = str(mqtt_msg).replace('}"','}')
        mqtt_msg = str(mqtt_msg).replace('"{','{')
        mqtt_msg = str(mqtt_msg).replace('{','')
        mqtt_msg = str(mqtt_msg).replace('}','')
        mqtt_msg = str(mqtt_msg).replace('"','')
        mqtt_msg = mqtt_msg.split(",")
        #print("====",mqtt_msg)
        #print("=----",mqtt_msg[0])
       
       
        #distance = mqtt_msg[0].split(":")[0]
       
     
        #print("distance:"+distance+"end")
        distance = mqtt_msg[0].split(":")[0]
        print("distance:"+distance+"end")
       
       
        mycursor = self.db.cursor()
        sql = 'INSERT INTO distance(distance) VALUES ('+distance+')'
        mycursor.execute(sql)
        self.db.commit()
##        print(ra)
        print(distance)
       
       
        print("Data Inserted!")


    def on_connect(self,mqttclient, userdata, flags,rc):
        if rc == 0:
            print("connected!")
        else:
            print("Connection failed")


    def on_message(self, mqttclient, userdata, msg):
        Thread(target=self.upload, args=(msg,)).start()


if __name__ == '__main__':
    Mqtt()
