from sklearn import svm
X = [[0.23, 0.13, 0.17], 
     [0.89, 0.92, 0.95]]
Y = [0, 1]
clasificador = svm.SVC()
clasificador.fit(X,Y)
