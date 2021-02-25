# -*- coding: UTF-8 -*-
from flask import Flask, request, abort
import DAN,csmapi, random, time, threading
from linebot import LineBotApi, WebhookHandler
from linebot.exceptions import InvalidSignatureError
from linebot.models import *

app = Flask(__name__)

# Channel Access Token
#line_bot_api = LineBotApi('YOUR CHANNEL ACCESS TOKEN')
line_bot_api = LineBotApi('ZNoMNLQIysaGk7XEGhtjrp7Z3k9xeLaJaSfUGnZ8lafjIkU47qM3nqSz75JicsuizOwRgC9eEYnqHUKzy3npb2sIY20JuauLiTfCWyVi8VobT/LpkmxILc6WOHBotY7e0WEUqs/UVBSThRlmhDTB2wdB04t89/1O/w1cDnyilFU=')
# Channel Secret
#handler = WebhookHandler('YOUR CHANNEL SECRET')
handler = WebhookHandler('69e225ffe8d485826af71a7e17ad74ae')

# connect to IoTtalk server
# ServerURL = 'http://XXX.XXX.XX.XX:XXXX'
# Reg_addr = None
ServerURL = 'http://140.114.77.90:9999'
Reg_addr = None
# Define your IoTtalk Device
DAN.profile
# Register
DAN.profile['dm_name'] ='109062578_demo'
DAN.profile['df_list'] = ['109062578_input_1','109062578_input_2','109062578_output']


# 監聽所有來自 /callback 的 Post Request
@app.route("/callback", methods=['POST'])
def callback():
	# get X-Line-Signature header value
	signature = request.headers['X-Line-Signature']
	# get request body as text
	body = request.get_data(as_text=True)
	app.logger.info("Request body: " + body)
	# handle webhook body
	try:
		handler.handle(body, signature)
	except InvalidSignatureError:
		abort(400)
	return 'OK'

@handler.add(MessageEvent, message=TextMessage)
def handle_message(event):
	message = TextSendMessage(text=event.message.text)
	Input = message.text.split(' ') 
	if  Input[0] == "Push":
		print('Push data to an input device feature')
		DAN.push('109062578_input_1', int(Input[1])) #Push data to an input device feature "109062578_input_1"
		print('109062578_input_1', int(Input[1]))
		DAN.push('109062578_input_2', int(Input[2])) #Push data to an input device feature "109062578_input_2"
		print('109062578_input_2', int(Input[2]))
		message.text = "Already push data to input device feature"
	elif Input[0] == "Pull":
		print('Pull data from an output device feature')
		ODF_data = DAN.pull('109062578_output') #Pull data from an output device feature "109062578_output"
		if ODF_data != None:
			print(ODF_data[0])
		message.text = "Already pull data from output device feature, Data is " + str(ODF_data[0])
	elif Input[0] == "Register":
		# Register 
		message.text = "Register"
		DAN.device_registration_with_retry(ServerURL,Reg_addr)

	elif Input[0] == "Deregister":
		# Deregister 
		message.text = "Deregister"
		line_bot_api.reply_message(event.reply_token, message)
		DAN.deregister()
		exit()
		

	line_bot_api.reply_message(event.reply_token, message)


import os
if __name__ == "__main__":



	port = int(os.environ.get('PORT', 5000))
	app.run(host='0.0.0.0', port=port)
