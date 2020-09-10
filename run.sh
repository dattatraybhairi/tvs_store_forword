#!bin/bash

sudo apt-get install mongodb

pip3 install pymongo==3.5.1

echo changing the file name 

sudo  cp social.py  Desktop/

sudo cp socialParams.json  Desktop/configuration/

sudo systemctl restart broker

sudo systemctl restart gateway

sudo systemctl restart counter

sudo systemctl status socialParamsConfig.service 

