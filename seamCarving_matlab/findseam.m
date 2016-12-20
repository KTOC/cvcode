%%%%%%%%%%%%%%%%
%
%ʹ�ö�̬�滮�ķ���ʵ��seam carving
%
%  ������ 2016.12.09 �Ͼ�
%
%%%%%%%%%%%%%%%%
clc;
clear;
Im=imread('ori.jpg');
gauss=fspecial('gaussian',[3,3],2);
Blur=imfilter(Im,gauss,'same');

[m,n,c]=size(Im);
%�Ҷ�ͼ
Gray=rgb2gray(Im);

%���ݶ�ͼ
hy=fspecial('sobel');
hx=hy';
Iy=imfilter(double(Gray),hy,'replicate');
Ix=imfilter(double(Gray),hx,'replicate');
Gradient=sqrt(Ix.^2+Iy.^2);
%��һ��
max1=max(max(Gradient)');
Gradient=Gradient./max1;

%����ֵͼ
Energy=zeros(m,n);
%·��ͼ
Path=zeros(m,n);
tmp=0;
for i=1:m
    for j=1:n
        if(i==1)
            Energy(i,j)=Gradient(i,j);
            Path(i,j)=0;
        else
            if(j==1)
                tmp=which_min2(Energy(i-1,j),Energy(i-1,j+1));
                Energy(i,j)=Gradient(i,j)+Energy(i-1,j+tmp);
                Path(i,j)=tmp;
            elseif(j==n)
                tmp=which_min2(Energy(i-1,j-1),Energy(i-1,j));
                Energy(i,j)=Gradient(i,j)+Energy(i-1,j-1+tmp);
                Path(i,j)=tmp-1;
            else
                tmp=which_min3(Energy(i-1,j-1),Energy(i-1,j),Energy(i-1,j+1));
                Energy(i,j)=Gradient(i,j)+Energy(i-1,j-1+tmp);
                Path(i,j)=tmp-1;
            end
        end
    end
end
%��һ��
%��һ��
max2=max(max(Energy)');
Energy=Energy./max2;

%������С·�������һ�е�������
lastCol=find(Energy(m,:)==min(Energy(m,:)));
col=lastCol(1);
%�軭���ָ���
Line=Im;
for i=m:-1:1
    Line(i,col,:)=[0,255,0];
    col=col+Path(i,col);
end

%����·���ϵĵ�
Im=Im(:);
for i=m:-1:1
    Im(1*i+col)=[];
    Im(2*i+col)=[];
    Im(3*i+col)=[];
    col=col+Path(i,col);
end
Im=reshape(Im,m,n-1,3);
%Gradient���ݶ�ͼ
figure,imshow(Gradient);title('Gradient Image');
%Energy���ۼ�����ͼ
figure,imshow(Energy);title('Cumulative Energy Image');
%Line�Ǳ�ע�˷ָ��ߵ�ͼ
figure,imshow(Line);title('Image with Seam');
%��Im�Ͻ��ָ����е�
figure,imshow(Im);title('after Cut Seam');