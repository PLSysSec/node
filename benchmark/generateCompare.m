function tests=generateCompareGraph(fpath)
%fpath='/home/shr/Code/benchmarks_303_buffer25runs_indexOf.txt'
table1 = readtable(fpath,'Delimiter',',','ReadVariableNames',false);
values1 = table2cell(table1);
values1 = values1(2:end, :);

table2 = table;
table2.binary = values1(:, 1);
table2.test = strrep(strcat(values1(:, 2),'-', values1(:,3)),'"','');
table2.rate = str2double(values1(:, 4));
table2.time = str2double(values1(:, 5));

table2Old = table2(strcmp(table2.binary(:,1), 'old'), :);
table2New = table2(strcmp(table2.binary(:,1), 'new'), :);

oldAvg = varfun(@mean,table2Old,'GroupingVariables','test','InputVariable','rate');
newAvg = varfun(@mean,table2New,'GroupingVariables','test','InputVariable','rate');
oldStd = varfun(@std,table2Old,'GroupingVariables','test','InputVariable','rate');
newStd = varfun(@std,table2New,'GroupingVariables','test','InputVariable','rate');

testCount = size(oldAvg, 1);

tableFinal = table;
tableFinal.testId = transpose(1:testCount);
tableFinal.oldAvg = oldAvg.mean_rate ./ oldAvg.mean_rate;
tableFinal.newAvg = newAvg.mean_rate ./ oldAvg.mean_rate;
tableFinal.oldStd = oldStd.std_rate ./ oldAvg.mean_rate;
tableFinal.newStd = newStd.std_rate ./ oldAvg.mean_rate;

arrFinal = table2array(tableFinal);

tests = table;
tests.Id = oldAvg.test;
tests.Separation = (arrFinal(:,3) - arrFinal(:,5)) - (arrFinal(:,2) + arrFinal(:,4));
tests = sortrows(tests, 'Separation', 'descend');
