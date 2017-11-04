class Test2{
	public static void main(String[] args) {
		System.out.println(new Third().getVal1());
	}
}

class First
{
	int a;
	int b;
	public int getVal1()
	{
		a=10;
		b=20;
		return a;
	}
	
	
}



class Second extends First{ 
	public int getVal2()
	{
		a=20;
		b=a+30;
		return b;
	}
}


class Third extends Second{
	First p1;
	Second p2;
	public int getVal1()
	{
		boolean c;
		int d;
		a=3;
		b=5;
		p1=new First();
		p2=new Second();
		System.out.println(p1.getVal1());
		System.out.println(p2.getVal1());
		System.out.println(p2.getVal2());
				
		c=a<b;
		if(c)
			d=2;
		else
			d=3;
		return d*10;
	}
}




