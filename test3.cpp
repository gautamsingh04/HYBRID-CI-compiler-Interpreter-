 a = 0;
 b = 1;  
 count = 0;
 next=0;
 n=5;
while(count < n) {
    print a; 
    next = a + b;
    a = b;
    b = next;
    count=count+1;
}