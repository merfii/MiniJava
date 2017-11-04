class Test4{
	public static void main(String[] args) {
		System.out.println(new Test41().op());
	}
}



class Test41
{
	int a;
	boolean b;
	public int op()
	{
		int b;
		int c;
		b=1;
		c=2;
		a=0;
		while(a<100)
		{
			a=a+1;
			if(b<c)
				b=c+a;
			else
				c=(2*a)+c;
			
		}

		return b;
	}

	
}

