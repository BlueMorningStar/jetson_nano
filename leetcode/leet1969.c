int minNonZeroProduct(int p) 
{
    //创建目标数组
    int len=pow(2,p)-1;
    long *nums=(long*)malloc(sizeof(long)*len);
    int flag=0;
    int m=0;
    long long res=1;
    for(int i=0;i<len;i++)
    {
        nums[i]=i+1;
    }
    for (int j=1;j<len;j++)
    {
        for(int k=len-1;k>j;k--)
        {
            flag=0;
            for(int n=60;n>1;n--)
            {
                if((nums[j]^nums[k])%((int)pow(2,n))==1)
                {
                    if(flag==0)
                    {
                        m=n;
                    }
                    flag=1;
                }
                else 
                {
                    if(flag==1)
                    {
                        flag=0;
                        m=0;
                        break;
                    }
                }
            }
            if(flag==1)
            {
                nums[j]=1;
                nums[k]=pow(2,m);
            }
            if(nums[j]==1) break;
        }
    }
    for(int i=0;i<len;i++)
    {
        res*=nums[i];
    }
    //return (int)(res%(int)(pow(10,9)+7));
    return (int);
}