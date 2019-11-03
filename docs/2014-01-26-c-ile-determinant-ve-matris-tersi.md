---
layout: post
title: C++ ile Determinant ve Matris Tersi
date: '2014-01-26T21:16:00.001+02:00'
author: Bahri ABACI
categories:
- Lineer Cebir
- Nümerik Yöntemler
modified_time: '2019-03-03T15:32:55.230+03:00'
thumbnail: /assets/post_resources/determinant/thumbnail.png
---

Lineer cebir, görüntü işleme, istatistik gibi pek çok alanda, problemleri bilgisayar programına dönüştürerek çözmede kolaylık sağlayan matrisler, sabit değerli sayıların bir düzen içerisinde tabloya yazılması ile oluşturulur. Ancak özellikle lineer cebir dersinin anlatımından kaynaklanan sebeplerden dolayı, her an hayatımızın içerisinde bulunan matrisler-le ilgili işlemlerin (Determinant hesaplama, Ters alma, Bölme) mantığını ve gerekliliğini pek çoğumuz bilmeyiz. Bu yazımda bir sebepten dolayı bu yazıyı okuyan sizlere Determinant ve  Matris Tersi / Bölmesi kavramlarının fiziksel karşılıkları ve Matris Tersi / Bölmesi için uygulanabilecek analitik yöntemleri anlatmaya çalışacağım.  
  
<!--more-->

Yazımın başlığı matris tersi almak olduğu için diğer kavramların uzun açıklamasından ve hesaplanması için yazılan algoritmalardan bu yazımda bahsetmeyeceğim. Ancak bu kavramlar hakkında fikir sahibi olmanın, bu konuda çalışanların veya çalışmak zorunda kalanların konuya olan ön yargısını yıkabileceğine inandığım için bu başlıklara da değineceğim.  
  

### Determinant:

  
Pek çok kaynakta verildiği tarifle determinant kare matrisler üzerinde tanımlanmış özel bir işlemdir! Peki neden bulunur determinant, fiziksel bir anlamı yok mudur? Tabi ki vardır ancak nedense bahsedilmez ve bu alanda çalışanlara ezber olarak öğretilir. Determinant (tutarlılık) bir matrisin tersinin olup olmadığı hakkında bilgi veren ve değeri girdi vektöründen elde edilmiş paralel kenarın (2 boyut için) hacmine eşit olan bir büyüklüktür. Önce geometrik olarak bu söylemimizi doğrulayalım ve $2\\times 2$ matris için yani iki adet vektör için herhangi bir formül kullanmadan determinant hesabını yapalım.  
  
![Determinant Hesaplama][determinant]
  
Öncelikle matrisin ne ifade ettiğini belirtelim. İlk sütun $x$ ekseninde $a$, $y$ ekseninde $c$ uzunluklu iki vektörün toplamından, ikinci sütun ise $x$ ekseninde $b$, $y$ ekseninde $d$ uzunluklu iki vektörün toplamından elde edilmiş iki vektörü gösteriyor. Bu vektörler kartezyen eksende $(0,0)$ dan başlayarak şekilde verildiği gibi (siyah çizgiler) gösterilir. Burada determinant mavi ile gösterilen paralel kenarın alanıdır ($3\times 3$ için hacim, $4\times 4$ için yüksek hacim ...).  
  
Şimdi bu alanı hesaplamaya çalışalım. İşleri kolaylaştırması için çizilen mor dikdörtgenden faydalanarak mavi ile belirtilen alanın ($M$); mor dikdörtgenin alanından ($P$), iki tane turuncu alan ($T$) ve iki tane yeşil alan ($Y$) eksiltilerek bulunabileceğini görürüz. Mor, turuncu ve yeşil bölgelerin alanları ise şu şekilde bulunabilir:  

$$  
\begin{aligned}
P &= \text{En}\times \text{Boy} &= \lvert a-b\lvert \lvert d+c\lvert \\  
T &= \frac{1}{2} \times \text{Taban Uzunluğu} \times \text{Yükseklik} &= \frac{1}{2}\lvert a\lvert \lvert c\lvert \\  
Y &= \frac{1}{2} \times \text{Taban Uzunluğu} \times \text{Yükseklik} &= \frac{1}{2}\lvert b\lvert \lvert d\lvert   
\end{aligned}  
$$

olarak bulunur. 

$P,T,Y$ alanları kullanılarak da mavi alan 

$$M = (a-b)(d+c) - 2(\frac{1}{2}ac+ \frac{1}{2}(-b)d) = ad+ac-bd-bc-ac+bd = ad-bc$$ 

olarak bulunur. (Burada $b$ nin negatif olması dolayısıyla yeşil alan bulunurken $b$ nin uzunluğu yani $\lvert b \lvert=-b$ kullanıldığına dikkat edelim.) 

Yani resimde de verilen matrisin determinantı bize ezberletilen formülle aynı olarak $\mathbb{ad-bc}$ bulunur. İfadeyi $3\times 3$ lük bir matris için hacim hesapları üzerinden tekrarlarsak da; 

$$A=\begin{bmatrix}a && b && c\\ d && e && f\\ g && h && i \end{bmatrix}$$ 

matrisi için, $\det(A) = (aei+bfg+cdh)-(ceg+bdi+afh)$ olarak (**Sarrus Kuralı** ile aynı) bulabiliriz.  

Bulduğumuz bu determinant değeri fiziksel olarak bir temele dayanmasına ek olarak, bize klasik cebir temellerinde yatan bir bilgiyi de getirmektedir. Örnek olarak $y=ax$ şeklinde bir cebirsel ifadeyi ele alalım. Burada $x$ ne olursa olsun $y$ $x$'in $a$ katı kadardır diyebiliyoruz.  
Aynı durumu matrisler için ele alacak olursak $Y=AX$ ifadesinde $Y$, $A$ nın kaç katıdır sorusuna $A$ katı şeklinde verilen bir ifade doğru olmasına karşın biraz anlamsızdır çünkü $A$ içerisinde pek çok sayı içeren bir matristir. İfadeyi bir boyutlu düzlem ile özleştiren ifade determinant fonksiyonu yardımıyla elde edilir.  
  
Determinantın fiziksel anlamını öğrendikten sonra şimdi de $0$ olması durumunu inceleyelim. Şekli referans olarak almaya devam edersek Mavi ile belirtilen alan yalnızca $e_1$ ve $e_2$ vektörlerinin birbirlerine paralel durumunda $0$ olacaktır.  
  
İşi anlamak için bildiğimiz sayı doğrusu ekseninden başlayalım. Sayı doğrusu üzerinde yapılan çarpma işleminin $T(x)=ax$ geometrik olarak ne anlama geldiğini görelim.  
  
![Sayı Doğrusu][sayi]

Yukarıda görüldüğü üzere sayı doğrusu üzerinde yer alan $e_1=(1,0)$ şeklindeki birim vektörümüz çarpma işlemi;sonucunda; $e_1=(4,0)$ vektörüne dönüşmüştür. Tek eksen üzerinde yapılan bu matris işleminin cebir temelli;çarpmadan bir farkı yoktur ve vektör uzunluğu dört kat artmıştır (Burada determinantın $4$ olduğuna dikkat edin). Şimdi amacımız sağ tarafta verilen $2$ birim genişlik ve $1$ birim yüksekliğe sahip alan için benzer bir sabit bulmak olsun. $e_1=(2,0)$ ve $e_1=(0,1)$ vektörlerini; $e_1=(3,0)$ ve $e_2=(0,2)$ vektörlerine dönüştüren $T$ matrisi $$T=\begin{bmatrix}1.5 && 0 \\ 0 && 2\end{bmatrix}$$ şekildedir. Tek boyutlu düzlemde uzunluk olan ölçümüzü iki boyutlu düzlemde alan olarak değiştirirsek; $T$ matrisinin determinantının ($\det(T)=2\times 1.5- 0 \times 0 =3$) girdi dikdörtgeninin alanını ($2\times 1=2$) **üç** kat artırarak çıktı dikdörgeninin alanına ($3\times 2=6$) yükselttiğini görürüz. Aynı geometrik yaklaşımı izleyerek $3\times 3$ bir $T$ matrisinin de girdi matrisinin hacmini $\det(T)$ kadar katladığını söyleyebiliriz.  
  

### Matris Tersi:

  
Bir matrisin tersi o matrisin çarpım sonucu götürdüğü düzlemde tanımlanan ve o düzlemin elemanları ile tek bir çarpım işlemi sonucu elamanı yola çıkılan düzleme geri döndüren matristir. İfadenin anlaşılması için üstteki resimleri inceleyebiliriz. $T$ ile çarpılması sonucu dört katına çıkan ilk vektörü, $(4,0)$ düzleminden $(1,0)$ düzlemine taşıyacak yeni $T^{-1}$ matrisine $T$ matrisinin tersi denir ve ifade bir boyutta tanımlandığı için $T^{-1}=\frac{1}{4}$ olarak kolaylıkla söylenebilir. Peki ikinci şekil için $T^{-1}$ nasıl bulunabilir? Öncelikle her bir nokta için yeni eksenlerini $T$ matrisini kullanarak  

$$  
\begin{bmatrix}
x'\\y'  
\end{bmatrix}=    
\begin{bmatrix}  
\frac{3}{2} && 0\\  
0 && 2  
\end{bmatrix}    
\begin{bmatrix}  
x\\  
y  
\end{bmatrix} \implies    
\begin{bmatrix}  
x'\\  
y'  
\end{bmatrix} =    
\begin{bmatrix}  
\frac{3}{2}x\\  
2y  
\end{bmatrix}  
$$  

Amacımız x' ve y' değerlerini kullanarak $x$ ve $y$ değerlerini bulmak olduğundan; $x=\frac{2}{3}x'$ ve $y=\frac{1}{2}y'$ denklem çiftini elde edebiliriz. Bu denklem çiftlerini $T^{-1}$ matrisini oluşturmak için matris şeklinde yazarsak; $$T^{-1}=\begin{bmatrix}\frac{2}{3} && 0 \\0 && \frac{1}{2}\end{bmatrix}$$olarak elde edilir. Dikkat edilmesi gereken ve şaşırtıcı olmayan bir nokta $TT^{-1}=I$ çarpımıdır. Bir boyutta $4\times \frac{1}{4} = 1$ olan bu çarpım iki boyutta $\begin{bmatrix}1 && 0\\0 && 1\end{bmatrix}$ şeklinde birim matrise eşit olmaktadır. Diğer bir önemli husus ise, sürekli karşımıza çıkan determinantın sıfır olması durumunda matris tersinin olmaması durumudur. Şu ana kadarki anlatımlarımı dikkatli bir şekilde okuduysanız bu durumun ne kadar açık bir durum olduğunu anlamışsınızdır. Eğer dönüşüm yapmak için kullandığımız $T$ matrisinin determinantı $0$ ise, dönüşüm sonrası oluşan şeklin (uzunluğu,alanı,hacmi,...) sıfır olacağından, gidilen bu düzlemden geri dönmek mümkün değildir çünkü sıfır ile hangi sayıyı çarparsak çarpalım $0$ dan farklı bir sayı elde etmek mümkün değildir.  
  

### Programlama:
  
Matris tersinin hesaplanması için yukarıda verdiğim örnek girdisi ve çıktısı bilinen bir durum için geçerliydi ve $Y=TX$ ile gidilen düzlemden $X=T^{-1}Y$ şeklinde bir $T^{-1}$ matrisiyle geri gelinebileceğini söylüyordu (Burada $T^{-1}=X/Y$ olduğuna dikkat edelim). Matris tersinin hesaplanması için geliştirilen yöntem ise girdi ve çıktı matrislerine ihtiyaç duymadan matris tersi alabileceğimiz ve aslında yukarıda da bahsettiğimiz bir yöntem; $T^{-1} = I/T$ (buradaki $I$ birim matristir), zaten başka bir şey beklemek de yanlış olurdu sanırım.  
  
Yöntemi analitik olarak hesaplamak için literatürde de sıklıkla kullanılan **Gauss-Jordan** yöntemini kullanacağız. Bu yöntemde bir $T$ matrisinin tersi $B=\left\[T\lvert I\right\]$ şeklinde oluşan bir $B$ matrisinin satır sütun işlemleri ile $B=\left\[I\lvert A\right\]$ şekline dönüştürülmesi ile elde edilen $A$ alt matrisi kullanılarak bulunur ($T^{-1}=A$). **Gauss-Jordan** yönteminin bir örnek ile anlaşılması en iyi yoldur ancak burada matrisleri yazmakta zorlandığım için, konunun anlaşılmasına yönelik bulduğum örnek üzerinden anlatımlı bir videoyu aşağıda veriyorum.  
  

Gauss-Jordan metodunun anlaşıldığını düşünerek kendi yazdığım kodu da aşağıda veriyorum. Verilen kod $Y/X$ mantığı ile çalıştığından tersi alınmak istenen matris $A$ ve $A$ ile aynı boyutlarda birim matris $B=I$ verilmelidir. Fonksiyona girdi olarak verilen M; $A$ matrisinin sütun sayısı, K ise $B$ matrisinin satır sayısıdır.

```c
double *matris_bol(double *A,double *B,int M,int K)
{
       int d,satir,sutun;
       double eps = 10e-10;
       double diag;
       
       double *matris = (double*) malloc(M*(M+K)*sizeof(double));
       double *sonuc  = (double*) malloc(M*K*sizeof(double));
       
       memcpy(matris,A,M*M*sizeof(double));
       memcpy(matris+M*M,B,M*K*sizeof(double));
       //matris = [A B];
       
       for(d=0; d<M; d++) {//diyagon üzerinde hareket edeceğiz matris[d][d]
              
              diag = matris[d*M+d]; 

              satir = d;
              sutun = d;
                     
              while(diag<eps && diag>-eps) {// uygun satırı bulana kadar ara
                     diag = matris[sutun+M*satir];
                     sutun++;
                     if(sutun==M) { free(matris); return NULL; }

                     if(diag<eps && diag>-eps) { continue;} // uygun satır bulundu ise satırları değiştir 
                     else {
                     
                     sutun--;

                            for(satir=0; satir<M+K; satir++) {                   
                                   double ara = matris[d+M*satir];            
                                   matris[d+M*satir] = matris[sutun+M*satir]; 
                                   matris[sutun+M*satir] = ara; }             


                     /* satırlar değiştikten sonra döngüden çık ve işlemlere devam et  */                                  
                     break;
                     }
              }
       
              for (satir = 0; satir < M; satir++) {
                     double katsayi = -matris[satir+M*d]/matris[d*M+d];

                     for(sutun=0; sutun<M+K; sutun++) { 
                     
                            if(satir!=d) { matris[satir+M*sutun] =  katsayi*matris[d+M*sutun] + matris[satir+M*sutun]; }
                     }
              }/*satır sutun for döngüsü sonu*/
       } /* diyagonal tarama bitişi*/
       
       for(satir=0; satir<M; satir++) {
       
                     double normalize = matris[satir+M*satir];
                     
                     for(sutun=0; sutun<M+K; sutun++) {         
                                   /*sonucu normalize et birim matris olur*/            
                                   matris[satir+M*sutun] =  matris[satir+M*sutun]/normalize; }
       }

       memcpy(sonuc, matris+M*M,M*K*sizeof(double));

       return sonuc;
}
```

Kodu adım adım incelemek gerekirse ilk yapılan işlemler kendi notasyonumuzla matris $\left\[A\lvert B\right\]$ şeklinde tutacağımız ve üzerinde satır/sütun işlemleri yapacağımız ve sonucu saklayacağımız sonuc göstericilerini tanımlamak. Ardından mavi ile gösterilen kısımda satır işlemleri yapılarak sol tarafta birim matris elde edilmeye çalışılmaktadır. Yazımızı kodun çalışmasına ilişkin bir örnekle bitirecek olursak;

```c
//1.sütun 2.sütun 3.sütun
double A[] = { 1,1,3 , 2,2,5,  3,2,7 };
//4.sütun 5.sütun 6.sütun
double B[] = { 1,0,0 , 0,1,0 , 0,0,1};

double *sonuc;
int satir,sutun;

sonuc = matris_bol(A,B,3,3);

for(satir=0; satir<3; satir++) { 
       for(sutun=0; sutun<3; sutun++) { 
              printf("x%d%d=%1.3f ",satir,sutun,sonuc[sutun+satir*3]); 
                                          } 
              printf("\n");
}
```

$A$ matrisinin tersini alıp ekrana yazdıran kod yukarıdaki şekilde olacaktır. Bu kodun ürettiği çıktı aşağıda verildiği gibidir.

![Matris Tersi Hesaplama C Kodu][kod_ekran]

Yazıyı yazarken faydalandığım iki önemli kaynağı sizler içinde referans olması için aşağıya ekliyorum, kavramların mantığını elimden geldiğince ve kendimce anlatmaya çalıştım umarım yardımcı ve açıklayıcı olmuştur.

**Referanslar**
* [Örneklerime temel oluşturan görselleri ve denklemleri içeren site](http://mathinsight.org/determinant_linear_transformation#lintrans3D)

* [Determinant kavramının tartışıldığı stackexchange sitesi](http://math.stackexchange.com/questions/668/whats-an-intuitive-way-to-think-about-the-determinant)

* [Gauss Eleminasyon yöntemin anlatıldığı wikipedia sayfası](https://www.wikiwand.com/en/Gaussian_elimination)

[RESOURCES]: # (List of the resources used by the blog post)
[determinant]: /assets/post_resources/determinant/determinant.png
[sayi]: /assets/post_resources/determinant/sayi.png
[kod_ekran]: /assets/post_resources/determinant/kod_ekran.png
