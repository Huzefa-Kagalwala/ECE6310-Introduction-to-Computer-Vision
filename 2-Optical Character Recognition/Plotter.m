%% INITIALIZE WORKSPACE

clear
close all
clc

%% PLOTTING
data = load('Data.txt');
fpr = data(:,7); tpr = data(:,6);
figure(1)
plot(fpr, tpr, 'k', 'LineWidth', 3)
xlabel('False Positive Rate (FPR)'), ylabel('True Positive Rate (TPR)');
title({'Complete ROC Curve','Threshold: 0-255'})

%% FINDING THE KNEE OF THE CURVE
thresh_array = data(:,1);
tn_array = data(:,4);
tp_array = data(:,2);
fn_array = data(:,5);
fp_array = data(:,3);
distance = zeros(256,1);
for i = 1:length(tpr)
    distance(i) = sqrt((fpr(i)-0)^2+(tpr(i)-1)^2);
end
mind = min(distance);
best_thresh_idx = find(distance == mind);
best_fpr = fpr(best_thresh_idx); best_tpr = tpr(best_thresh_idx);
best_thresh = thresh_array(best_thresh_idx);
best_tn = tn_array(best_thresh_idx);
best_tp = tp_array(best_thresh_idx);
best_fn = fn_array(best_thresh_idx);
best_fp = fp_array(best_thresh_idx);

%% PLOTTING
figure(2)
plot(fpr(171:221), tpr(171:221), 'k', 'LineWidth', 3)
xlabel('False Positive Rate (FPR)'), ylabel('True Positive Rate (TPR)');
txt1 = 'Optimal point on the curve';
txt2 = 'FPR: %0.4f TPR: %0.4f';
text(best_fpr+0.015, best_tpr, sprintf(txt1), 'color', 'k', 'FontSize', 10, 'FontWeight', 'bold');
text(best_fpr+0.015, best_tpr-0.01, sprintf(txt2, best_fpr, best_tpr), 'color', 'k', 'FontSize', 10, 'FontWeight', 'bold');
hold on
plot(best_fpr, best_tpr, 'ko', 'LineWidth', 5, 'MarkerSize', 5)
hold off
title({'Zoomed in ROC Curve','Threshold: 170-220'})
%xlim([0 1]), ylim([0 1]);