import urllib.request as req
import time
import bs4 
def write_command_func():
	
	path = 'website.txt'
	f = open(path, 'r')
	a=1
	command_char = 'command'
	txt='.txt'
	for url in f.readlines():
		
		print(url)
		request=req.Request(url,headers={
		    "User-Agent":"Mozilla/5.0 (X11; Linux x86_64; rv:91.0) Gecko/20100101 Firefox/91.0"
		    #"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.64 Safari/537.36"
		})

		path1 = command_char+str(a)+txt
		command = open(path1, 'w')
		

		with req.urlopen(request) as response:
		    data = response.read().decode("utf-8")
		root = bs4.BeautifulSoup(data, "html.parser")
		titles = root.find_all("div",class_="draft-block draft--p left")
		for title in titles:
			if title.string != None:
				print(title.string, file=command)	
		command.close()
		
		print("next website")
		a+=1
		
				
				
	f.close()	
if __name__ == '__main__':

    time_counter = 60
    while(1):
    
    	print(time_counter)
    	if(time_counter == 60):
    		write_command_func()
    		time_counter = 0
    	
    	time.sleep(1)
    	time_counter+=1

    

