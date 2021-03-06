% Parse files. Not necessary if files already exist.
train_file = '.\train_2000.csv';
test_file = '.\test_2000.csv';
systemCommand = 'C:\Python27\python27.exe generate_train_test_data_2.py 1900';
system(systemCommand);

% Generate training features
fid=fopen(train_file);
tline = fgetl(fid);
num_col = length(find(tline==',')) + 1;
format = ['%d %s' repmat('%f', 1, num_col - 12)];
c = textscan(fid, format, 'Delimiter', ',');
fclose(fid);

% Convert labels
train_features = c(:, 3:end);
train_features = cell2mat(train_features);

orig_labels = unique(c{2});
label_dict = containers.Map(orig_labels, 1:length(orig_labels));
id_to_string = containers.Map(1:length(orig_labels), orig_labels);

train_labels = zeros(length(c{2}), 1);
for i = 1 : length(c{2})
    train_labels(i) = label_dict(c{2}{i});
end

%==Generate test features, in same order as training features===
fid2=fopen(test_file);
tline2 = fgetl(fid2);
num_col2 = length(find(tline2==',')) + 1;
format2 = ['%d %s' repmat('%f', 1, num_col2 - 12)];
c2 = textscan(fid2, format2, 'Delimiter', ',');
fclose(fid2);

test_features = cell2mat(c2(:, 3:end));
test_ids = c2{1};

%=========================PCA Analysis=====================================
%[COEFF,SCORE] = princomp(train_features);
%train_centered = SCORE*COEFF';
% ==================svm=================
%train_features = train_features(:, 1:100);
%svm_train_features = train_features(:, 1500:2000);
%svm_train_labels = train_labels(:);

% Cross validation accuracy. Use as test before submission. 4^(-3) 4^(-2)
svm_classifier_cv = svmtrain(train_labels, train_features, ['-h 0 -q -v 5 -c 8 -t 2 -g ' num2str(1/32)]);  % used 4 for c; RBF kernel function C = 4, gamma = 4^-3

%poly_cv_accu = svmtrain(train_labels, train_features, '-h 0 -q -c 0.25 -v 5 -t 1 -d 1');  % Polynomial
%svm_accu = svmpredict(test_label, test_data, svm_classifier);
%svm_accuracy = svm_accu(1);
%results(t, 3) = svm_accuracy;
% =============================ensemble====================================
%ens_classifier = fitensemble(train_data, train_label, 'AdaBoostM1', 100,
%'Tree'); % LogitBoost, Subspace, Bag ; 'AllPredictorCombinations' ; 'KNN'
%'Discriminant' , 'type', 'classification'
% ens_classifier = fitensemble(train_features, train_labels, 'Bag', 100, 'Tree', 'type', 'classification');
% ens_cv = crossval(ens_classifier, 'KFold', 5);
% ens_error_rate = kfoldLoss(ens_cv);
% ens_predictions = predict(ens_classifier, test_data);
% ens_accuracy = 1 - (nnz(ens_predictions - test_label)/length(test_label));
% results(t, 4) = ens_accuracy;
% =================================KNN=====================================
% knn_classifier = fitcknn(train_features, train_labels);
% knn_cv = crossval(knn_classifier, 'KFold', 5);
% knn_error_rate = kfoldLoss(knn_cv);
%=============================Discriminant Analysis========================
% disc_anal_classifier = fitcdiscr(train_features, train_labels); 
% disc_anal_cv = crossval(disc_anal_classifier, 'KFold', 5);
% disc_anal_error_rate = kfoldLoss(disc_anal_cv);
%============================Logistic Regression===========================
% lr_train_features = train_features(1:19999, 1:100);
% lr_train_labels = train_labels(1:19999);
% log_reg_classifier = mnrfit(lr_train_features, lr_train_labels);
%log_reg_predictions = mnrval(log_reg_classifier, test_data);
% log_reg_cv = crossval(log_reg_classifier, 'KFold', 5);
% log_reg_error_rate = kfoldLoss(log_reg_cv);
% [~,index_max_prob] = max(log_reg_predictions, [], 2);
% lg_accuracy = 1 - (nnz(index_max_prob - lg_test_label)/length(lg_test_label));
% results(t, 1) = lg_accuracy;
%===========================Naive Bayes============================
% nb_classifier = fitcnb(train_features,train_labels);
% nb_cv = crossval(nb_classifier, 'KFold', 5);
% nb_error_rate = kfoldLoss(nb_cv);

% CVDiscrMdl = fitcdiscr(X,Y,Name,Value)
% CVEnsMdl = fitensemble(X,Y,Name,Value)
% CVKNNMdl = fitcknn(X,Y,Name,Value)
% CVNBMdl = fitcnb(X,Y,Name,Value)
% CVSVMMdl = fitcsvm(X,Y,Name,Value)
% CVTreeMdl = fitctree(X,Y,Name,Value)

%===========================Predictions===================================
%disc_anal_predict = predict(disc_anal_classifier, test_features);

% Run following when generating submission file. Modify file name as
% necessary.
svm_classifier = svmtrain(train_labels, train_features, ['-h 0 -q -c 8 -t 2 -g ' num2str(1/32)]);  % RBF kernel function C = 4, gamma = 4^-3
dummy = zeros(9944, 1);
[svm_predictions, accuracy, ~] = svmpredict(dummy, test_features, svm_classifier);
prediction_labels = cell(length(svm_predictions), 1);
for i = 1 : length(svm_predictions)
    prediction_labels{i} = id_to_string(svm_predictions(i));
end
out_file = fopen('submission12.csv','w');
fprintf(out_file,'id,cuisine\n');
for i = 1 : length(svm_predictions)
    fprintf(out_file,'%d,%s\n', test_ids(i), prediction_labels{i});
end
fclose(out_file);