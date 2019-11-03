---
layout: post
title: Perspektif Dönüşümü
date: '2013-12-08T00:26:00.001+02:00'
author: Bahri ABACI
categories:
- Lineer Cebir
- Nümerik Yöntemler
modified_time: '2015-09-16T12:16:29.887+03:00'
thumbnail: /assets/post_resources/perspective_transform/thumbnail.png
---

Perspektif nesnenin bulunduğu konuma bağlı olarak, gözlemcinin gözünde bıraktığı etkiyi (görüntüyü) 2 boyutlu bir düzlemde canlandırmak için geliştirilmiş bir iz düşüm tekniğidir. Rönesans döneminde Masaccio' nun resimlerinde kullanmaya başladığı teknik günümüzde gerçekçi çizimler oluşturmak için olmazsa olmazlardandır. Ancak olayı görüntü işleme açısından ele aldığımızda **perspektif** genellikle işlerimizi zorlaştıran bir etkiye sahiptir. Amaç nesne tanıma veya sınıflandırma olduğunda nesnenin hangi açıdan görüntülendiğinin bir önemi olmamalıdır.  

<!--more-->
  
Örneğin standart bir karakter tanıma uygulamasına görüntü olarak verilen "*cescript*, "_cescript_" veya ʇdıɹɔsǝɔ görüntüleri aynı kelime olmalarına rağmen bilgisayar tarafından her seferinde farklı olarak okunacaktır. İşte **perspektif** dönüşümü bu noktada devreye girerek verilen görüntü üzerindeki ölçekleme, yayma, dönme ve kayma gibi etkileri kaldırabilmemizi sağlar. 

Perspektif düzeltmede amaç kişinin veya nesnenin konum değiştirmesi sonucu oluşacak etkiyi betimleyebilmektir. Bu işlem sayesinde görüntü oluştuktan sonra dahi belirli kısıtlar içerisinde resme baktığımız açıyı değiştirebiliriz. Algoritma karakter tanıma uygulamalrında (Cam Scanner gibi) çekilmiş bir görüntüyü belirli kalıplar içerisine oturmada, plaka tanıma, yüz tanıma gibi uygulamalarda normalizasyon sırasında sıklıkla kullanılmaktadır.  

Aşağıda verilen matriste $x$,$y$ herhangi bir koordinat değeri olmak üzere, $x'$ ve $y'$ bu iki değerin dönüşüm sonrası değerlerini göstermektedir. Matris gösterimindeki a değerlerinin her birinin özel bir anlamı vardır. 

$$
\begin{bmatrix}
x'z \\
y'z \\
z
\end{bmatrix}  
\begin{bmatrix}
a_{11} & a_{12} & a_{13}\\  
a_{21} & a_{22} & a_{23}\\  
a_{31} & a_{32} & a_{33}\\  
\end{bmatrix}  
\begin{bmatrix}
x\\
y\\
1\\
\end{bmatrix}  
$$

Şimdi basitlik olması için bazı $a$ değerlerini $0$ kabul ederek elde edeceğimiz $x'$ ve $y'$ değerlerini yorumlamaya çalışalım. Anlaşılması en kolay durum ile incelememize başlayabiliriz: $a12=a13=a22=a23=a31=a32 = 0$ ve $a11=a22=a33 = 1$. Bu durumda $x' = x$ ve $y'=y$ olacaktır. Yani yeni oluşan görüntüdeki her bir nokta olduğu yerde kalacaktır. (Aynalama)

### Öteleme

Diğer basit bir durum ise $a12=a22=a31=a32 = 0$ ve $a11=a22=a33 = 1$ durumudur. Bu durumda yeni koordinat değerleri $y' = y + a23$, $x' = x + a13$ şeklinde olacaktır. Yani yeni görüntüdeki herbir nokta $a13$ kadar sağa ve $a23$ kadar aşağıya kayacaktır.

### Ölçekleme

Ölçekleme işlemi için $a12=a13=a22=a23=a31=a32 = 0$ ve ${a33} = 1$ seçilerek şu durum elde edilir: $y' = a22 y$ , $x' = a11 x$. İfadenin daha anlaşılır olması için  $a22=1,a11=2$ seçelim, bu durumda yeni oluşan resimde $y$ değerleri değişmezken, $x=10$ daki bir nokta $x'=20$ ye, $x=20$ deki bir nokta $x'=40$ a ... şeklinde her bir $x$ noktası $2$ katına gitmekte yani resim $x$ ekseninde iki kat ölçeklenmektedir.

### Döndürme

Döndürme işlemi $a11,a12,a21,a22$ değerlerinin özel şekilde seçilmesi ile yapılır. Bu seçim döndürme açısı $\theta$ ya bağlı olarak şu şekilde ifade edilir: $a11 = \cos(\theta)$ , $a12 =-\sin(\theta)$ , $a21 = \sin(\theta)$ , $a22 = \cos(\theta)$. İnceleme yapabilmemiz için yine basit olarak $\theta$ değerini $180$, $a13,a23,a31,a32 = 0$ ve $a33 = 1$ seçelim. $\cos(180)=-1$ ve $\sin(180) = 0$ olduğu göz önünde bulundurularak dönüşüm sonrası değerlerimiz $x' = -x$, $y' = -y$ şeklinde olacaktır. Yani yeni görüntüde $y$ noktaları $-y$, $x$ noktası $-x$ noktasına taşınacak ve görüntü orijine göre simetrisi alınmış yani $180$ derece döndürülmüş olacaktır.

Daha karmaşık durumlar için ilk olarak $a13,a23$ değerlerinin sıfırdan farklı olduğu durumlar incelenebilir. Bu durumda $x'$ değerinde $y$, $y'$ değerinde ise $x$ ekseninin etkisi görünecektir. Daha karmaşık bir durum ise **perspektif düzeltmenin** temeli olan $a31,a32$ değerlerini sıfırdan farklı seçmektir. Bu durum görüntüdeki uzaklık etkisini $x'$ ve $y'$ değerlerine yayarak gözde üç boyut etkisini yaratan bükümü düzeltmeyi sağlar.

Bu yazımda basit örneklerle başlık halinde de verdiğim üç dönüşümü (Öteleme-Ölçekleme-Döndürme) örneklerle incelemeye çalışacağım. Dönüşüm için yazılan kodu inceleyerek başlayalım.  
  
```c
//a11 = T[0]; a12 = T[3]; a13 = T[6];
//a21 = T[1]; a22 = T[4]; a23 = T[7]; 
//a31 = T[2]; a32 = T[5]; a33 = T[8];

//x' = (x*a11 + y*a12 + a13) / z';
//y' = (x*a21 + y*a22 + a23) / z';
//z' = (x*a31 + y*a32 + a33);

double xa11[im.bminfo.width+1];
double xa21[im.bminfo.width+1];
double xa31[im.bminfo.width+1];
double ya12[im.bminfo.height+1];
double ya22[im.bminfo.height+1];
double ya32[im.bminfo.height+1];
    
for(i=0; i &lt im.bminfo.width+1; i++) {
                xa11[i] = i*T[0];
                xa21[i] = i*T[1];
                xa31[i] = i*T[2]; }
for(j=0; j &lt im.bminfo.height+1; j++) {
                ya12[j] = j*T[3];
                ya22[j] = j*T[4];
                ya32[j] = j*T[5]; }         
    
for(i=0; i &lt im.bminfo.width; i++) {
            for(j=0; j &lt im.bminfo.height; j++) {
                        
                x = (xa11[i] + ya12[j] + T[6]) / (xa31[i] + ya32[j] + T[8]);
                y = (xa21[i] + ya22[j] + T[7]) / (xa31[i] + ya32[j] + T[8]);
                        
                x_tam = (int) x;
                y_tam = (int) y;
                        
                if(x_tam &lt 0 | x_tam &gt Wmax-1 | y_tam &lt 0 | y_tam &gt Hmax-1) { continue; }
                        
                out.pixels[x_tam][y_tam].red   = im.pixels[i][j].red;
                out.pixels[x_tam][y_tam].green = im.pixels[i][j].green;
                out.pixels[x_tam][y_tam].blue  = im.pixels[i][j].blue;
                }
}

return out;                 
```
  
Kod içerisindeki ilk 7 satırda verilen yorumlardan yapılan işlemin anlatım ile benzetimi yapılabilir. Dikkat çekebilecek olan tek nokta `xa11` ile başlayan değişkenler olabilir. Son yazılarımda sıklıkla yapmaya çalıştığım gibi bu da hızlandırma için yapılmış bir işlem. Bu değişkenleri tanımlayarak bellekte tuttuğum alanı artırmama rağmen döngü içerisinde aynı çarpma işlemlerini tekrar ve tekrar yapmaktan kaçındığım için kodun özellikle büyük resimlerde çalışma süresi oldukça kısalıyor. Bir diğer nokta göze çarpan Wmax ve Hmax değişkenleri, bu değişkenler çıktı olarak oluşturulacak resmin boyutlarını söylüyor.

**Peki bu kadar mı?**

Değil tabi ki :) Şu ana kadar okuduğunuz yazı içerisinde dikkatinizi bir noktanın özellikle çekmesini istemiştim. Ölçekleme kısmında verdiğim örnekte katsayının iki seçilmesi durumunda resimdeki her bir noktanın iki katına gittiğini söylemiştim. Aynı örnekte durumu yeni oluşan resim açısından ele alırsak, $x=1,x=3,\cdots$ gibi noktalar hiçbir $x,y$ çifti için bir değere sahip olamayacaktır. Bu yüzden oluşan resimde siyah noktalar (bu örnek için yatay çizgiler) oluşacaktır. Bunu engellemek için şöyle bir yöntem izleyebiliriz. 

$$
\begin{bmatrix}
xz \\
yz \\
z
\end{bmatrix}  
\begin{bmatrix}
ia_{11} & ia_{12} & ia_{13}\\  
ia_{21} & ia_{22} & ia_{23}\\  
ia_{31} & ia_{32} & ia_{33}\\  
\end{bmatrix}  
\begin{bmatrix}
x'\\
y'\\
1\\
\end{bmatrix}  
$$
  
Yöntem ilk kısımda anlatılan yöntem ile aynı görünmesine karşılık, bu sefer çıktı görüntüsü üzerindeki bir noktanın $(x',y')$ girdi görüntüsü üzerinde nereden geldiğini aradığımızdan $(x,y)$ çıktı görüntüsü üzerinde hiç bir boş nokta kalmayacaktır. Kod üzerinde yapacağımız en önemli değişim a katsayıları yerine ia katsayılarını bulma kısmında olacaktır. Matrissel şekilde gösterilen $ia$ katsayıları $a$ katsayılarından oluşan matrisin tersine ait elemanlardır. Matris tersi bulma işlemini başka bir yazımda anlatmayı planladığımdan bu kısmı geçerek yeni yönteme ait kodları paylaşıyorum.  
  
```c
//a11 = T[0]; a12 = T[3]; a13 = T[6];
//a21 = T[1]; a22 = T[4]; a23 = T[7]; 
//a31 = T[2]; a32 = T[5]; a33 = T[8];

//x' = (x*a11 + y*a12 + a13) / z';
//y' = (x*a21 + y*a22 + a23) / z';
//z' = (x*a31 + y*a32 + a33);

double xa11[Wmax+1];
double xa21[Wmax+1];
double xa31[Wmax+1];
double ya12[Hmax+1];
double ya22[Hmax+1];
double ya32[Hmax+1]; 

for(i=0; i < Wmax+1; i++) 
{
    xa11[i] = i*iT[0];
    xa21[i] = i*iT[1];
    xa31[i] = i*iT[2]; 

}

for(j=0; j < Hmax+1; j++) 
{
    ya12[j] = j*iT[3];
    ya22[j] = j*iT[4];
    ya32[j] = j*iT[5]; 
}         
    
for(i=0; i < Wmax; i++) 
{
    for(j=0; j < Hmax; j++) 
    {
        x = (xa11[i] + ya12[j] + iT[6]) / (xa31[i] + ya32[j] + iT[8]);
        y = (xa21[i] + ya22[j] + iT[7]) / (xa31[i] + ya32[j] + iT[8]);
                
        x_tam = (int) x;
        y_tam = (int) y;
                
        if(x_tam < 0 | x_tam > im.bminfo.width-1 | y_tam < 0 | y_tam > im.bminfo.height-1) { continue; }
                
        out.pixels[i][j].red   = im.pixels[x_tam][y_tam].red;
        out.pixels[i][j].green = im.pixels[x_tam][y_tam].green;
        out.pixels[i][j].blue  = im.pixels[x_tam][y_tam].blue;
    }
}

return out;
```
  
Dikkat edilecek olursa kodlamadaki tek değişimin T matrisi yerine iT olmadığı görülür. Bahsettiğim üzere bu değişim sonrasında for döngülerimizi çıkış görüntüsü üzerindeki koordinatlarda döndüreceğimizden x\_tam ve y\_tam (yani x' ve y') değerleri giriş görüntüsünde yerine yazılacaktır. Artık basitten karmaşığa doğru örneklerimize geçebiliriz.

![Affine Dönüşümü][affine]

Son söz olarak perspektif düzeltmedeki katsayıların otomatik bulunmasından bahsedebiliriz. Bu yazıyı yazmamdaki esas neden **sudoku çözücü** için gerekli olan perspektif düzeltmesinin nasıl yapılacağını anlatmaktı. [Bir önceki yazımı](http://www.cescript.com/2013/12/sudoku-cozucu-uygulamasi.html) okuduysanız, sudoku karesinin karşıdan çekilmediği durumlarda ne olacağını merak etmiş olabilirsiniz. Böyle bir durumla karşılaşmamız durumunda gerekli dönüşüm katsayılarını otomatik olarak belirleyerek dönüşüm yapmamız gerekecektir.

## Perspektif Dönüşüm Katsayılarının Elde Edilmesi

Parametrelerin otomatik olarak çıkarılması için bir kaç matris bölmesi işlemi gerekmektedir. Böylece girdi olarak verilen bir görüntü otomatik olarak kolaylıkla hizalanabilmektedir. 
Perspektif dönüşümünün bir matris çarpması ile yapıldığını biliyoruz. Dönüşüm için kullandığımız matrisi işlemlerimiz için tekrar yazalım.

$$
\begin{bmatrix}
x'z \\
y'z \\
z
\end{bmatrix}  
\begin{bmatrix}
a_{11} & a_{12} & a_{13}\\  
a_{21} & a_{22} & a_{23}\\  
a_{31} & a_{32} & a_{33}\\  
\end{bmatrix}  
\begin{bmatrix}
x\\
y\\
1\\
\end{bmatrix}  
$$
  
Burada $(x',y')$ çiftlerinin $(x,y)$ çiftlerinin $a$ matrisi ile dönüşüm sonrası aldığı değerler olduğunu hatırlayalım. Amacımızın $a$ matrisini bulmak olduğunu tekrarlayarak işlemlerimize başlayalım. İlk olarak $z$ değerini $z=a_{31}x+a_{32}y+a_{33}$ şeklinde yazarak $x'$ ve $y'$ değerlerini açık şekilde yazmaya çalışalım.
  
$$x'=\frac{a_{11}x+a_{12}y+a_{13}}{a_{31}x+a_{32}y+a_{33}}; y'=\frac{a_{21}x+a_{22}y+a_{23}}{a_{31}x+a_{32}y+a_{33}}$$  
  
Şimdi içler dışlar çarpımı yaparak ifadeleri düzenleyelim ve kolaylık olması adına $a_{33}=1$ alalım.  
  
$$a_{31}xx' + a_{32}yx' + x' = a_{11}x+a_{12}y+a_{13}$$ve $$a_{31}xy' + a_{32}yy' + y' = a_{21}x+a_{22}y+a_{23}$$  
  
Elde edilen ifadeyi solda $x',y'$; sağda ise $a'$ lü terimler olacak şekilde düzenlersek;  
  
$$x' = a_{11}x+a_{12}y+a_{13} - a_{31}xx' - a_{32}yx'$$ve $$y' = a_{21}x+a_{22}y+a_{23} - a_{31}xy' -a_{32}yy'$$ elde edilir.  
  
Bu denklemleri matris formunda yazacak olursak ($B = A\times a$);  
  
$$ 
\begin{bmatrix} 
x'\\
y'  
\end{bmatrix}  
=  
\begin{bmatrix}
x & y & 1 & 0 & 0 & 0 & -xx' & -yx'\\  
&&&&&&&\\  
0 & 0 & 0 & x & y & 1 & -xy' & -yy'  
\end{bmatrix}  
\begin{bmatrix}
a_{11}\\  
a_{12}\\  
a_{13}\\  
a_{21}\\  
a_{22}\\  
a_{23}\\  
a_{31}\\  
a_{32}\\  
a_{33}\\  
\end{bmatrix}   
$$ 

ifadesi elde edilir. Burada aranılan $a$ vektörü $a = (A^{-1})B = B/A$ matris bölme işlemi ile kolaylıkla bulunabilir. Burada dikkat edilmesi gereken bir nokta elimizde sekiz bilinmeyen ($a$ vektörünün elemanları) olamasına rağmen görünürde sadece iki denklemimiz olmasıdır. Sekiz bilinmeyenli bir denklemin tek çözümünün olması için bağımsız sekiz denklem gerektiğinden bizimde çalışmada tek bir $(x,y)$ noktası yerine dört tane $(x,y)$ noktası üzerinden dönüşüm yapılarak $a$ matrisi bulunmuştur.

Şimdi gelelim kodlama kısmına. Bir önceki yazıda matris tersini bulma kısmından bahsettiğim için yapmamız gerek tek işlem `A` ve `B` matrislerini hazırlamak.

```c
double A[] = 
    {   x1,  0, x2,  0, x3,  0, x4,  0,
        y1,  0, y2,  0, y3,  0, y4,  0,
        1,  0,  1,  0,  1,  0,  1,  0,
        0, x1,  0, x2,  0, x3,  0, x4,
        0, y1,  0, y2,  0, y3,  0, y4,
        0,  1,  0,  1,  0,  1,  0,  1,
        -X1*x1, -Y1*x1, -X2*x2, -Y2*x2, -X3*x3, -Y3*x3, -X4*x4, -Y4*x4,
        -X1*y1, -Y1*y1, -X2*y2, -Y2*y2, -X3*y3, -Y3*y3, -X4*y4, -Y4*y4
    };     

//4.sutun
double B[] = {X1,Y1,X2,Y2,X3,Y3,X4,Y4}; 
```

Matrislerimizi oluşturduktan sonra katsayıları bulmak için önceki yazımızda kullandığımız `matris_bol` fonksiyonunu kullanacağız, ardından oluşan $8$ satırlık vektörü kullanarak $3\times 3$ lük a matrisimizi yeniden oluşturacağız.

```c
sonuc = matris_bol(A,B,8,1);

a =
    {   
        sonuc[0],sonuc[3],sonuc[6],
        sonuc[1],sonuc[4],sonuc[7],
        sonuc[2],sonuc[5],   1    
    };
```

Bu aşamada istediğimiz noktaları istediğimiz yeni noktalara dönüştürecek dönüşüm matrisini elde etmiş bulunuyoruz. Bu değerler kullanılarak istenilen dönüşüm `resim_rot` fonksiyonunu ile gerçekleştirilebilir.

Her zamanki gibi bir örnek uygulama ile yazımızı bitirelim. Birkaç yazı önce temelini attığımız sudoku çözücü uygulamamız için bulunan sudoku karesini hizalamaya çalışalım.

```c
sonuc = matris_bol(A,B,8,1);

x1=116, x2=464, x3=501, x4=33 ;
y1=66 , y2=67 , y3=378, y4=352;

X1=1, X2=512, X3=512, X4=1  ;
Y1=1, Y2=1  , Y3=512, Y4=512;

double A[] = {  x1,  0, x2,  0, x3,  0, x4,  0,
y1,  0, y2,  0, y3,  0, y4,  0,
1,  0,  1,  0,  1,  0,  1,  0,
0, x1,  0, x2,  0, x3,  0, x4,
0, y1,  0, y2,  0, y3,  0, y4,
0,  1,  0,  1,  0,  1,  0,  1,

-X1*x1, -Y1*x1, -X2*x2, -Y2*x2, -X3*x3, -Y3*x3, -X4*x4, -Y4*x4,
-X1*y1, -Y1*y1, -X2*y2, -Y2*y2, -X3*y3, -Y3*y3, -X4*y4, -Y4*y4
};     
//4.sutun
double B[] = {X1,Y1,X2,Y2,X3,Y3,X4,Y4};

double *sonuc;
int satir,sutun;

sonuc = matris_bol(A,B,8,1);

a ={ sonuc[0],sonuc[3],sonuc[6],
sonuc[1],sonuc[4],sonuc[7],
sonuc[2],sonuc[5],   1    };

giren = resim_oku("sudoku.bmp");
out   = resim_rot(giren,a,512,512);

resim_yaz(out,"sudoku_hizalanmis.bmp");
```
  
Yazılan kod okunan sudoku resminin dört köşesini girdi olarak aldıktan sonra sudoku karesini $512 \times 512$ lik bir karenin içerisine hizalayacak dönüşüm matrisini bulur ve dönüşümü gerçekleştirir. Kodumuzun girdi ve çıktıları şu şekilde olacaktır.

![Perspektif Transform][sudoku]

Başka bir uygulama olarak da rubik küp örneğine bakalım. Burada amacımız verilen rubik küpe yukarıdan baksaydık nasıl bir görüntü elde ederdik sorusuna cevap bulmak.

![Perspektif Transform][rubik]

Görüldüğü gibi algoritma sadece köşe noktalarını alarak otomatik ürettiği parametreler ile gayet başarılı sonuçlar üretmekte. Bu da bize kamera açısından bağımsız görüntü işleme uygulamalarında büyük bir katkı sağlamakta.

**Referanslar**

[RESOURCES]: # (List of the resources used by the blog post)
[affine]: /assets/post_resources/perspective_transform/affine.png
[sudoku]: /assets/post_resources/perspective_transform/sudoku.png
[rubik]: /assets/post_resources/perspective_transform/rubik.png
