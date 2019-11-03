---
layout: post
title: Özdeğer ve Özvektörler (Eigenvalues and Eigenvectors)
date: '2019-03-26T23:41:00.000+03:00'
author: Bahri ABACI
categories:
- Lineer Cebir
- Nümerik Yöntemler
modified_time: '2019-07-14T23:07:47.135+03:00'
thumbnail: /assets/post_resources/eigen_values/thumbnail.png
---

Önceki başlıklarda doğrusal cebirin önemli kavramları olan determinant ve matris tersi bulma konularına değinmiştik. Burada determinant işleminin matrisle ilişkilendirilmiş bir sabit olduğunu ve $2\times 2$ bir matris için bu sabitin matrisi oluşturan vektörler arasında kalan alanı, $3\times 3$ bir matris içinse matrisi oluşturan vektörler arasında kalan hacmi ifade ettiğini söylemiştik. Matris çarpması işlemi için de vektörleri ne şekilde ölçeklendirdiği ve döndürdüğünden bahsetmiştik. Bu yazımızda ise doğrusal cebrin bir başka önemli tanımlamalarından biri olan özdeğer (eigenvalue) ve özvektör (eigenvector) kavramları üzerinde duracağız.

<!--more-->

Yazımızın konusu olan özdeğer ve özvektörler de, determinant gibi matrisle ilişkilendirilmiş özel değerlerdir. Bu kavramları anlamak için matris çarpması işlemine geri dönelim. $\vec{y}=A\vec{x}$ gibi bir çarpma işlemi sonucunda; hemen herzaman $\vec{x}$ vektöründe ölçek, açı ve kayma değişimi meydana gelir. Bu değişim $\vec{y}$ vektöründe tutulduğundan rastgele bir $A$ matrisi seçip basit bir $\vec{x}=[1, -1]^T$ için vektörün nasıl değiştiğini inceleyelim.

$$
y=v
\left [
\begin{array}{cc}
a_{11} & a_{12}\\
a_{21} & a_{22}\\
\end{array}
\right ]
\left [
\begin{array}{r}
1\\
-1\\
\end{array}
\right ]
=
\left [
\begin{array}{c}
a_{11}-a_{12}\\
a_{21}-a_{22}\\
\end{array}
\right ]
$$

Burada $A$ matrisine bağlı olarak $\vec{x}$ vektörü dönme ve ölçekleme işlemleri ile $\vec{y}=[a_{11}-a_{12},a_{21}-a_{22}]^T$ vektörüne dönüşmüştür. Peki öyle vektörler bulunabilir mi ki; çarpım sonucunda $\vec{x}$ vektörünün sadece ölçeği değişsin, yönü değişmesin.

En basit çözüm olarak $a_{11}-a_{12} \; = \; a_{22}-a_{21}\; = \; \lambda$ gibi bir sabit seçersek;

$$
\vec{y}=
\left [
\begin{array}{cc}
a_{11} & a_{12}\\
a_{21} & a_{22}\\
\end{array}
\right ]
\left [
\begin{array}{r}
1\\
-1\\
\end{array}
\right ]
=
\left [
\begin{array}{r}
\lambda\\
-\lambda\\
\end{array}
\right ]
= \lambda \vec{x}
$$

işlemi sonucunda $x$ vektörünün doğrultusu değişmemiş, yalnızca genliği (ölçeği) $\lambda$ kadar değişmiş olacaktır. Böyle bir değişime neden olan tek değişim $a_{11}-a_{12} = a_{22}-a_{21} = \lambda$ dır. Ancak $A$ matrisinin $a_{11}-a_{12} = a_{22}-a_{21} = \lambda$ denklemini sağlayan sonsuz sayıda çözümü vardır. Örneğin $\lambda=-1$ için aşağıda verilen tüm $A$ matrisleri, $A\vec{x}$ çarpımında $\vec{x}$ e skalar olarak davranır ve $\vec{x}$ vektörünü $\lambda$ kadar ölçekler. 

$$A=\left[\begin{array}{rr}0&&1\\-2&&-3\end{array}\right], \left[\begin{array}{rr}0&&1\\3&&2\end{array}\right], \left[\begin{array}{rr}2&&3\\4&&3\end{array}\right], \dots$$

Herhangi bir $A$ matrisi için, $\vec{y}=A\vec{x}=\lambda \vec{x}$ şartını sağlayan tüm $\lambda$ değerlerine $A$ matrisinin özdeğeri, $\vec{x}$ vektörlerine ise $A$ matrisinin özvektörleri denir. Dikkat edilirse $\vec{x}$, $A$ matrisinin bir özvektörü olduğunda, $A\vec{x}$ çarpımında $A$ matrisi, $A=\lambda$ olan bir sabit gibi davranmaktadır.

Peki herhangi bir $A$ matrisi için, bu şartı yerine getiren $\vec{x}$ vektörleri ve $\lambda$ değerleri nasıl bulunur. Aslıdan çözüm oldukça basit. $A\vec{x}=\lambda \vec{x}$ eşitliğinde sağ tarafı sola atıp $\vec{x}$ parantezine alırsak: $$(A-\lambda I)\vec{x}=\vec{0} \label{1}\tag{1}$$ buluruz. $\vec{v}\neq\vec{0}$ olması durumunda, eşitliğin sağlanması için $A-\lambda I$ ifadesinin $\vec{x}$ vektörünü $\vec{0}$ noktasına götürmesi gereklidir. $A-\lambda I$ ifadesinin tersi olduğunu varsayarak, yukarıda verilen eşitliğin her iki tarafını da $(A-\lambda I)^{-1}$ ile çarparsak: $$(A-\lambda I)^{-1}(A-\lambda I)\vec{x}=(A-\lambda I)^{-1}\vec{0} \equiv \vec{x}=\vec{0}$$ olduğunu görürüz. Yani $(A-\lambda I)$ matrisinin tersi bulunabilir ise; denklemin tek çözümü $\vec{x}=\vec{0}$ dır. Aradığımız özvektörlerin $\vec{0}$ dan farklı olmasını istediğimizden $(A-\lambda I)$ ifadesinin tersinin **bulunamaması** gerekmektedir. Bu da determinant yazımızdan hatırlayacağımız üzere; $$\det(A-\lambda I)=0$$ olması durumunda mümkündür. Bu ifadeye $A$ matrisinin karakteristik denklemi denir. Karakteristik denklemin çözümü ile önce özdeğerler, bu özdeğerlerin $(\ref{1})$ denkleminde yerine yazılması sonucunda çıkan ifade çözülerek de özvektörler bulunur. 

**<span style="color: red;">NOT: </span>Burada; $\vec{x_1}$, $A$ matrisinin bir özvektörü ise $\alpha \vec{x_1}$ de $(\ref{1})$ eşitliğini sağlayacağından, $\alpha \vec{x_1}$'in de $A$' nın bir öz vektörü olduğuna dikkat edilmelidir.**

Örnek bir $A=\left[\begin{array}{rr}0&&1\\-2&&-3\end{array}\right]$ matrisini ele alarak özdeğer ve özvektör bulma işlemini tekrar açıklayalım. $A$ matrisinin karakteristik denklemi: $$\det(A-\lambda I) =\begin{vmatrix}0-\lambda&&1\\-2&&-3-\lambda\end{vmatrix}=0$$ şeklinde yazılır. Özdeğerleri bulmak için determinant ifadesi polinom şeklinde yazılarak elde edilen $\lambda^2+3\lambda+2 = 0$ ikinci dereceden denklem çözülürse; özdeğerler $\lambda_1=-1$, $\lambda_2=-2$ olarak bulunur. Bu özdeğerlerden ilki ($\lambda_1$)' nin $(\ref{1})$ de yerine yazılması ile; 
$$
\left[\begin{array}{rr}
0- (-1)&&1\\
-2&&-3 - (-1)
\end{array}\right]
\vec{x}
= \vec{0}
$$
bulunur. İfadenin çözümü sonucunda da $\vec{x_1}=\alpha\left[\begin{array}{r}1\\-1\end{array}\right]$ bulunur. $\lambda_2$ nin $(\ref{1})$ de yerine yazılması ile de; 
$$
\left[\begin{array}{rr}
0- (-2)&&1\\
-2&&-3 - (-2)
\end{array}\right]
\vec{x}
= \vec{0}
$$
eşitliği elde edilir. Eşitliğin çözülmesi sonucunda da $\vec{x_2}=\alpha\left[\begin{array}{r}1\\-2\end{array}\right]$ bulunur.

<hr align="center" color="yellow" size="2" width="100%" /><span style="color: yellow;">DİKKAT: </span>

Matlab, Octave gibi matris işleme kütüphanelerinde özdeğer ve özvektörlerin hesaplanması için `eig` fonksiyonu bulunmaktadır. Çözümünü elimizle yaptığımız $A=\left[\begin{array}{rr}0&&1\\-2&&-3\end{array}\right]$ matrisinin özdeğer ve özvektörler bu programlar aracılığı ile bulunmak istenirse komut satırına 

`[v,lambda]=eig([0 1;-2 -3])` komutu yazılmalıdır. Ancak bu işlem sonucunda programın $\vec{v_1}=\left[\begin{array}{r}0.70711\\-0.70711\end{array}\right]$ ve $\vec{v_2}=\left[\begin{array}{r}-0.44721\\-0.89443\end{array}\right]$ şeklinde hesaplandığını görürüz.
```matlab
v =

   0.70711  -0.44721
  -0.70711   0.89443

lambda =

  -1   0
   0  -2
```
Bu sonuçlar $\alpha_1=\frac{1}{||x_1||}$ ve $\alpha_2=\frac{1}{||x_2||}$ seçilmesi ile elde edilir ve böylece öz vektörlerin normlarının $||v_1||=||v_2||=1$ olması sağlanır.

<hr align="center" color="yellow" size="2" width="100%" />

Literatürde özdeğer ve özvektör hesaplamak için pek çok yöntem (power iteration, bisection method, QR algorithm, Jacobi eigenvalue algorithm, vs.) önerilmiştir. Ancak nümerik yöntemler ile özdeğer ve özvektör hesaplama işlemi sanıldığından daha karmaşık bir problemdir ve önerilen bu yöntemlerden çoğu bazı özel matris yapıları (gerçek simetrik matris, hermitian matris, tridiagonal matris, vb.) için kullanılabilirdir. IMLAB görüntü işleme kütüphanesinde bulunan `eig` fonksiyonu **Jacobi Özdeğer Algoritması** ile özdeğer ve özvektörleri bulmaktadır.

### Jacobi Özdeğer Algoritması
Jacobi özdeğer algoritması **Gerçek ve Simetrik Matrisler** üzerinde çalışan iteratif bir yöntemdir. Yöntem her bir iterasyonda uygun bir rotasyon matrisi $G$ ile $A$ matrisinin köşegendışı bir elemanı sıfırlamaya çalışır ve yeterli sayıda iterasyon sonrasında matrisi köşegen matrise dönüştürür. Rotasyon işlemi ile özdeğerler değişmediğinden iterasyonlar sonucu elde edilen köşegen matrisin her bir köşegen elemanı orjinal matrisin bir özdeğeridir. $G$ özel seçilmiş bir rotasyon matrisi olmak üzere Jacobi iterasyonu şu şekilde ifade edilir: $$A^{t+1} =  G^{t}A^{t}{G^{t}}^{\intercal}$$

Burada $G$ <button onclick="clickToRevealGivensRotation()">Given rotasyon matrisi</button> olarak bilinmektedir ve uygulandığı herhangi bir matrisi veya vektörü $(i,j)$ düzleminde $\theta$ kadar döndürmek için kullanılmaktadır.

<script>function clickToRevealGivensRotation() {   var x = document.getElementById("clickToRevealGivensRotation");   if (x.style.display === "none") {     x.style.display = "block";   } else {     x.style.display = "none";   } } </script>

<div id="clickToRevealGivensRotation" style="display: none;">
$$
G(i,j,\theta) = 
\left[
\begin{array}{cccccccc}
&& \color{red}i && \color{red}j  & &\\
1 & \cdots & 0 & \cdots & 0 & \cdots & 0 & \\
\vdots & \ddots & \vdots &  & \vdots & & \vdots & \\
0 & \cdots & \cos(\theta) & \cdots & -\sin(\theta) & \cdots & 0 & \color{red}i\\
\vdots &  & \vdots & \ddots & \vdots & & \vdots &\\
0 & \cdots & \sin(\theta) & \cdots & \cos(\theta) & \cdots & 0 & \color{red}j \\
\vdots &  & \vdots &  & \vdots & \ddots & \vdots &\\
0 & \cdots & 0 & \cdots & 0 & \cdots & 1 &\\
\end{array}\right]
$$
</div>

Bu matris oldukça ayrık yapıda olduğundan Jacobi iterasyonu sonucunda $A^{t}$ matrisinin sadece $i,j$ satır ve sütunlarında değişiklik meydana gelir ve $s=\sin(\theta_t)$ ve $c=\cos(\theta_t)$ olmak üzere $A^{t+1}$ matrisi aşağıdaki şekle dönüşür. 

$$
\begin{array}{cclc} 
A^{t+1}_{ii}&=&c^2A^{t}_{ii} -2scA^{t}_{ij} + s^2A^{t}_{jj}&\\
A^{t+1}_{jj}&=&s^2A^{t}_{ii} +2scA^{t}_{ij} + c^2A^{t}_{jj}&\\
A^{t+1}_{ij} = A^{t+1}_{ji}&=&(c^2-s^2)A^{t}_{ij} +sc(A^{t}_{ii} - A^{t}_{jj})&\\
A^{t+1}_{ik} = A^{t+1}_{ki}&=&cA^{t}_{ik} -sA^{t}_{jk} & k \neq i,j\\
A^{t+1}_{kj} = A^{t+1}_{jk}&=&sA^{t}_{ik} +cA^{t}_{jk} & k \neq i,j\\
A^{t+1}_{kl} & = & A^{t+1}_{kl} & k,l \neq i,j
\end{array}
$$


$$\theta_t = \tan^{-1}(\frac{2A^{t}_{ij}}{A^{t}_{jj}-A^{t}_{ii}})/2$$ seçilmesi durumunda $A^{t+1}_{ij} = 0$ olacağından, $A$ matrisinin köşegen dışı bir elemanı $0$ yapılmış olunur. Bu işlem tüm köşegen dışı elemanlar için tekrarlanarak $A$ matrisi köşegenleştirilebilir.

**<span style="color: red;">NOT: </span> __Her dönüşüm işlemi sonrasında $A^{t+1}_{ij}$ elemanı sıfır yapılırken, daha önceki iterasyonlarda sıfır yapılan bir elemanın değeri değişebilir. Ancak her sweep ($\frac{N(N-1)}{2}$ iterasyon) sonucu köşegendışı elemanların her zaman küçüldüğü kanıtlanmıştır.**

### Kovaryans - Özdeğer & Özvektör İlişkisi
Özdeğer ve özvektör analizi, büyük verilerin görselleştirilmesinde ve boyut indirgeme işlemlerinde sıklıkla kullanılan yöntemlerdendir. Büyük bir veri setinin kovaryans matrisinin özdeğer ve özvektör analizi yapıldığında, özvektörler verinin en yüksek değişintiyi gösterdiği düzlemi, özdeğerler ise bu düzlemdeki değişinti miktarını göstermektedir. Bu işlem liteartürde **Temel Bileşen Analizi (Principal Component Analysis)** olarak bilinir. 

Aşağıda solda verilen görselde $\mu=\left[\begin{array}{cc}0 & 0 \end{array}\right]$, $\sigma^2=\left[\begin{array}{cc} 2 & 1.2\\ 1.2 & 1\end{array}\right]$ olmak üzere; $X \sim N(\mu,\sigma^2)$ dağılımından $200$ adet örnek alınmış ve gösterilmiştir. Verilen kovaryans matrisinin özdeğer ve özvektörlerini **Jacobi Özdeğer Algoritması** ile bulmaya çalışalım ve çıkan sonucu görsel üzerinde inceleyelim. 

* **Adım 0:** $A^t=\left[\begin{array}{rr} 2 & 1.2\\ 1.2 & 1\end{array}\right]$, $V^t=\left[\begin{array}{rr} 1 & 0\\ 0 & 1\end{array}\right]$ olarak iterasyona başlanır

* **Adım 1:** $A^t$ nin köşegendışı elemanını $A(1,2)=0$ yapacak $\theta$ açısı bulunur
$$\theta_t = \frac{\tan^{-1}(\frac{2A^{t}_{ij}}{A^{t}_{jj}-A^{t}_{ii}})}{2} = \frac{\tan^{-1}(2\frac{1.2}{1-2})}{2}=56.310^\circ$$ 

* **Adım 2:** $G(1,2,\theta_t)$ rotasyon matrisi hesaplanır
$$G_t = \left[\begin{array}{rr}  0.55470 & -0.83205\\ 0.83205 & 0.55470\end{array}\right]$$ 

* **Adım 3:** $A^{t+1} = G^{t}A^{t}{G^{t}}^{\intercal}$ çarpımı ile $A_{t+1}$ köşegen matrisi elde edilir
$$A^{t+1} =  {\left[\begin{array}{rr}  0.55470 & -0.83205\\ 0.83205 & 0.55470\end{array}\right]}\left[\begin{array}{rr} 2 & 1.2\\ 1.2 & 1\end{array}\right]{\left[\begin{array}{rr}  0.55470 & -0.83205\\ 0.83205 & 0.55470\end{array}\right]}^{\intercal}=\left[\begin{array}{rr} 0.2 & 0\\ 0 & 2.8\end{array}\right]$$ 
* **Adım 4:** $V^{t+1} = V^{t}G^{t}$ çarpımı ile özvektörler hesaplanır

$$V^{t+1} = \left[\begin{array}{rr} 1 & 0\\ 0 & 1\end{array}\right]{\left[\begin{array}{rr}  0.55470 & -0.83205\\ 0.83205 & 0.55470\end{array}\right]}^{\intercal} =\left[\begin{array}{rr} 0.55470 & 0.83205\\ -0.83205 & 0.55470\end{array}\right]$$ 
Görüldüğü üzere girdi matrisimizin boyutu $2\times2$ büyüklüğünde olduğu için özdeğer ve özvektör bulma işlemi $5$ adımda tamamlanmıştır. Algoritma sonucunda özdeğerler $\lambda_1=0.2$ ve $\lambda_2=2.8$ olarak bulunmuştur. 
Bu özdeğerlere karşı gelen özvektörler ise $\vec{v_1}=\left[\begin{array}{r}0.55470 \\ -0.83205\end{array}\right]$ ve $\vec{v_2}=\left[\begin{array}{r}0.83205 \\ 0.55470\end{array}\right]$ olarak bulunmuştur. 

<button onclick="clickToRevealAnalticalSolution()">Aynı problemin analitik yöntemler ile çözümü için tıklayın</button>
<script>function clickToRevealAnalticalSolution() {   var x = document.getElementById("clickToRevealAnalticalSolution");   if (x.style.display === "none") {     x.style.display = "block";   } else {     x.style.display = "none";   } } </script>

<div id="clickToRevealAnalticalSolution" style="display: none;"><hr align="center" color="black" size="2" width="100%" />

Verilen girdi matrisinin karakteristik denklemi:
$$\det(A-\lambda I) =\begin{vmatrix}2-\lambda & 1.2\\1.2 & 1-\lambda\end{vmatrix}=0$$şeklinde yazılır. Özdeğerleri bulmak için determinant ifadesi polinom şeklinde yazılır ise $\lambda^2-3\lambda+0.56=0$ elde edilir. Bu denklemin kökleri $\lambda_1=0.2$ ve $\lambda_2=2.8$ dir. Bu özdeğerlerden ilki ($\lambda_1$)' nin $(\ref{1})$ de yerine yazılması ile; 
$$
\left[\begin{array}{rr}
2- (0.2) & 1.2\\
1.2 & 1 - (0.2)
\end{array}\right]
\vec{x}
= \vec{0}
$$
bulunur. İfadenin çözümü sonucunda da $\vec{x_1}=\alpha\left[\begin{array}{r}0.55470 \\ -0.83205\end{array}\right]$ bulunur. $\lambda_2$ nin $(\ref{1})$ de yerine yazılması ile de; 
$$
\left[\begin{array}{rr}
2- (2.8) & 1.2\\
1.2 & 1 - (2.8)
\end{array}\right]
\vec{x}
= \vec{0}
$$
eşitliği elde edilir. Eşitliğin çözülmesi sonucunda da $\vec{x_2}=\alpha\left[\begin{array}{r}0.83205 \\ 0.55470\end{array}\right]$ bulunur.

<hr align="center" color="black" size="2" width="100%" /></div>

Bu vektörler aşağıdaki resimde sırasıyla mavi ve yeşil olarak gösterilmiştir.

Görselden de görüleceği üzere, rastgele dağılımlı bir verinin kovaryans matrisinin özvektörleri, verinin en yüksek saçılımı (varyasyonu) gösterdiği eksenleri vermektedir. Özdeğerler ise verinin bu eksenler üzerinde ne kadar saçıldığının ölçüsünü belirtmektedir. Bu gözleme ait ispatlar ve pratik kullanımına ait örnekler **Temel Bileşen Analizi (Principal Component Analysis)** yazımızda incelenecektir.

**Referanslar**
* Press, William H., et al. [Numerical recipes 3rd edition: The art of scientific computing.](https://e-maxx.ru/bookz/files/numerical_recipes.pdf) Cambridge university press, 2007.

* Jacobi eigenvalue algorithm [wikipedia page](http://www.wikiwand.com/en/Jacobi_eigenvalue_algorithm)

* [Geometric interpretation of covariance matrix](http://www.visiondummy.com/2014/04/geometric-interpretation-covariance-matrix)

