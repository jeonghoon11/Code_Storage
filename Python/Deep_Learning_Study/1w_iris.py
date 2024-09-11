import pandas as pd #data anaylsis
import matplotlib.pyplot as plt #visualization
from sklearn import datasets
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.neighbors import KNeighborsClassifier
from sklearn import metrics

iris = datasets.load_iris() #dataset -> 값 불러옴

data=pd.DataFrame(iris.data)

X = iris.data
y = iris.target

# (80훈련:20정답 비율로 분할
X_train,X_test,y_train,y_test = train_test_split(X,y,test_size = 4)
#X와 Y값이 대응되게 섞음

#X_train : X데이터의 80퍼   , X_test : 20퍼 예측 시키기 위해서 필요한 값


knn = KNeighborsClassifier(n_neighbors=6)   #AI 학습 과정
knn.fit(X_train, y_train)   #AI 학습 과정

#AI에게 나머지 20프로 주고 y_pred에 저장
y_pred = knn.predict(X_test)

scores = metrics.accuracy_score(y_test, y_pred) #test : 정답(20프로)
print(scores)

# print(data)



plt.scatter(data.iloc[:,[0]], data.iloc[:,[1]], c=iris.target)
plt.show()

# print(iris.DESCR)   # 데이터셋의 정보'''