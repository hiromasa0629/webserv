import requests
import json
from colorama import Fore
import sys

URL = "http://localhost:8080"

def	print_response(res):
	print("---------------------------------------------------------")
	print(Fore.GREEN + "Request" + Fore.RESET)
	print(res.request.method, res.request.url)
	print(json.dumps(dict(res.request.headers), indent=2))

	print("\n")
	print(Fore.GREEN + "Response" + Fore.RESET)
	print(res.status_code)
	print(json.dumps(dict(res.headers), indent=2))
	print(res.text)

def get_request(path = ""):
	res = requests.get(url=URL + path)
	print_response(res)

def post_request(path = ""):
	res = requests.post(url=URL + path)
	print_response(res)

def post_request_image(path = ""):
	files = {'file': open('assets/OIP.jpeg', 'rb')}
	res = requests.post(url=URL + path, files=files)
	print_response(res)

def main():
	if (len(sys.argv) != 3):
		print("python3 client.py [method] [URI]")
		return
	if (sys.argv[1] == "GET"):
		get_request(sys.argv[2])
	elif (sys.argv[1] == "POST"):
		if (sys.argv[2] == "/upload"):
			post_request_image(sys.argv[2])
		else:
			post_request(sys.argv[2])

if __name__ == "__main__":
    main()