%VLOOKUP.M
function b=vlookup(m,lut,n)
% 
% Return matrix b which has equal size as m and the values are taken from
% from the n column of lookup table lut.This is matrix version 
% of VLOOKUP similar to MS Excel function.
%
% example: 
% m = [ 1 2 0; 3 4 1] 
% lut = [0 0.5 12; 1 0.8 25; 2 0.4 32; 3 0.7 41]
% 
% m = [ 1 2 0;
%       3 4 1]
% lut = [0 0.5 12;
%        1 0.8 25;
%        2 0.4 32;
%        3 0.7 41]
% b=vlookup(m,lut,3) 
%  => [25 32  12;
%      41 0 25]
% b=vlookup(m,lut,2) 
%  => [0.8 0.4  0.5;
%      0.7 0    0.8]
% 
% (c) 2006 by Kardi Teknomo http://people.revoledu.com/kardi/
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Note: The following code work fine and fast
% find value of m that equal to the value of first column 
% the lut table and replace it with the value of column n
% if cannot find, return NaN
b=interp1(lut(:,1),lut(:,n),m);

% replace NaN with zero
b(isnan(b))=0;        


% The following is another version but slower
% [u,v]=size(m);
% [w,z]=size(lut);
% b=zeros(u,v);
% for i=1:u
%     for j=1:v
%         q=m(i,j);
%         for k=1:w
%             if q==lut(k),
%                 b(i,j)=lut(k,n);
%                 break
%             end
%         end
%     end
% end
    
% The following is yet another version but much slower
% [u,v]=size(m);
% m1=reshape(m,u*v,1);
% h=[];
% for i=1:length(m1)
%     g=find(lut==m1(i));
%     if isempty(g), 
%         h=[h; 0];
%     else
%         h=[h;lut(g(1),n)];
%     end 
% end
% b=reshape(h,u,v);
