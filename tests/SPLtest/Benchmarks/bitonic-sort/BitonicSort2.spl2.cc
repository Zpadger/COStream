int N = 8;
/* true for UP sort and false for DOWN sort */ 
int sortdir = 1;  

composite CompareExchange(output stream<int x>Out, input stream<int x>In)
{
	param
		int sortdir;
	
	Out=CompareExchange(In)
	{
		work
		{
			 int k1, k2, mink, maxk; 
		
			k1 = In[0].x; 
			k2 = In[1].x; 
			if (k1 <= k2)
			{  
				mink = k1; 
				maxk = k2; 
			} 
			else /* k1 > k2 */ 
			{ 
				mink = k2; 
				maxk = k1; 
			} 
 
			if (sortdir != 0) // lxx
			{ 
				/* UP sort */
				Out[0].x=mink;
				Out[1].x=maxk;
			} 
			else /* sortdir == false */ 
			{ 
				/* DOWN sort */ 
				Out[0].x=maxk;
				Out[1].x=mink;
			}
		}
		window 
		{
			Out	tumbling(2);
			In	sliding(2,2);
		}
	};
}

composite PartitionBitonicSequence(output stream<int x>Out, input stream<int x>In)
{
	param
		int L,int sortdir;
	int i;
	Out=splitjoin(In)
	{
		 split roundrobin();
		for (i=0; i<(L/2); i++) 
		 add CompareExchange(sortdir); 
		join roundrobin();
	};
}

composite StepOfLastMerge(output stream<int x>Out, input stream<int x>In)
{
	param
		int L,int numseqp,int sortdir;
	int j;
	Out=splitjoin(In)
	{
		 split roundrobin(L);
		 for(j=0; j<numseqp; j++)
		 {
			 if(L>2)
				 add PartitionBitonicSequence(L, sortdir); 
			 else
				 add CompareExchange(sortdir); 
		 }
		join roundrobin(L);	
	};
}

composite StepOfMerge(output stream<int x>Out, input stream<int x>In)
{
	param
		int L,int numseqp,int dircnt;
		
	
	int temp[numseqp];
	int i,j;
	for(i=0; i<numseqp; i++)
	{
		if((i/dircnt)%2==0 )
			temp[i]=1;
		else
			temp[i]=0;
	}
		
	
	Out=splitjoin(In)
	{
		split roundrobin(L);
		for (j=0; j<numseqp; j++)
		{
			if(L>2)
				add PartitionBitonicSequence(L, temp[j]); 
			else
				add CompareExchange(temp[j]); 
		}
		join roundrobin(L);
	};
}

composite MergeStage(output stream<int x>Out, input stream<int x>In)
{
	param
		int P,int N;
	int i;
	Out=pipeline(In)
	{
		for(i=1; i<P; i=i*2)
			add StepOfMerge(P/i, (N/P)*i, i);
	};
		
}

composite LastMergeStage(output stream<int x>Out, input stream<int x>In)
{
	param
		int N,int sortdir;
		
	int i;
	Out=pipeline(In)
	{
		for(i=1;i<N;i=i*2)
			add StepOfLastMerge(N/i, i, sortdir);
	};
		
}

composite BitonicSortKernel(output stream<int x>Out, input stream<int x>In)
{
	param
		int N,int sortdir;
		
	
	stream<int x>OutP;
	int i;
	OutP=pipeline(In)
	{
		for(i=2; i<=(N/2); i=2*i)
			add MergeStage(i, N); 
			 
	};
		
	Out = LastMergeStage(OutP)(N, sortdir);
}
composite Main()
{
	stream<int x> KeySource,BSK;
	int i;
	KeySource = Source()
	{
			
		int A[8] = {5,3,6,7,9,4,8,2};
		
		work
		{
			for (i=0; i<8; i++)
				KeySource[i].x = A[i];
		}
		
		window {
			KeySource tumbling(8);
		}
	};
		
	BSK= BitonicSortKernel(KeySource)(N,sortdir);
		
	Sink(BSK)
	{		
		work
		{	
			for (i=0; i<8; i++)
				println(BSK[i].x);
		}
		
		window {
			BSK sliding(8,8);
		}
	};
}