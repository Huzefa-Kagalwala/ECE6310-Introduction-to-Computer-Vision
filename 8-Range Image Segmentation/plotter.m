
clear
load('Output.csv');
figure()
arwlen = sqrt(Output(:,4).^2+Output(:,5).^2+Output(:,6).^2);
quiver3(Output(:,1), Output(:,2), Output(:,3), Output(:,4)./arwlen, Output(:,5)./arwlen, Output(:,6)./arwlen)