examples = [1.0 0.9 0.8 0.7 0.6 0.5 0.4 0.3 0.2 0.1; 1 2 1 1 2 1 2 2 1 2];
n = size(examples, 2);
pClass = 1;
new = sortrows(examples', 1, 'descend')';
tp = 0; fp = 0;
p = length(find(new(2,:) == pClass));
precision = zeros(1, n); recall = zeros(1, n);
aucPr = zeros(1, n); ap = zeros(1, n);
for i = 1 : n
    if new(2, i) == pClass
        tp = tp + 1;
    else
        fp = fp + 1;
    end
    precision(i) = tp / i;
    recall(i) = tp / p;
    if i == 1
        aucPr(i) = recall(i) * (precision(i) + 1) / 2;
        ap(i) = recall(i) * precision(i);
    else
        aucPr(i) = (recall(i) - recall(i - 1)) * (precision(i) + precision(i - 1)) / 2;
        ap(i) = (recall(i) - recall(i - 1)) * precision(i);
    end
end
disp('AUC-PR'); disp(aucPr); disp(sum(aucPr)); disp('AP'); disp(ap); disp(sum(ap));