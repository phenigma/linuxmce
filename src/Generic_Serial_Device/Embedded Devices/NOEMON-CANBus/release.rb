conn_.Send("C\r");
resp=conn_.Recv(1,2000);
if(resp != "\n")
	print "Error 3\n";
	return;
end
print "Channel closed\n"
