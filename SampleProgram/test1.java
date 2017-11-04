class Test1{
	public static void main(String[] args) {
		System.out.println(new Test11().getVal1());
	}
}



class Test11
{
	int a;
	int b;
	public int inc()
	{
		a=a+1;
		return a;
	}


	public int getVal1()
	{
		a=0;
		b=0;
		while((b<1) &&	(this.inc())<3)
		{
 			b=b+1;
		}
		return a;
	}

	
}
