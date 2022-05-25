import urllib.request as req
import bs4 
def write_command_func():
	
	path = 'command.txt'
	f = open(path, 'w')
	url="https://vocus.cc/article/6285fba1fd89780001230aff"
	request=req.Request(url,headers={
	    "User-Agent":"Mozilla/5.0 (X11; Linux x86_64; rv:91.0) Gecko/20100101 Firefox/91.0"
	    #"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.64 Safari/537.36"
	})

	with req.urlopen(request) as response:
	    data = response.read().decode("utf-8")
	root = bs4.BeautifulSoup(data, "html.parser")
	titles = root.find_all("div",class_="draft-block draft--p left")
	for title in titles:
		if title.string != None:
			print(title.string, file=f)	
	f.close()	
if __name__ == '__main__':
    write_command_func()

