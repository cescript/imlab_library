---
layout: post
title: İmge Renklendirme (Image Colorization)
date: '2016-04-14T21:11:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
- Lineer Cebir
- Nümerik Yöntemler
modified_time: '2016-04-14T21:15:15.749+03:00'
thumbnail: /assets/post_resources/image_colorization/thumbnail.png
---

Kısa bir süre önceki [İmge Renksizleştirme yazımızda](http://www.cescript.com/2015/10/imge-renksizlestirme-image.html)
renkli imgelerin gri seviye dönüşümü için yenilikçi bir yöntem
incelemiştik. Yöntem üç farklı kanaldan (R,G,B) oluşan renkli sayısal
bir imgeyi tek bir kanala indirirken doku bilgisinin korunmasınını da
güvence altına almaktaydı. Bu yazımızda ise önceki yöntemin tersi olarak
İmge Renklendirmeyi ele alacağız. İmge renklendirme (Image colorization)
gri seviye kodlanmış bir imgeden renkli imge elde etmeye yarayan bir
yöntemdir. Genellikle görselliği iyileştirmek için kullanılsa da gri
seviye kameradan / sensörden alınan görüntülerin renklendirilmesi, eski
fotoğrafların canlandırılması veya imge renklerinin değiştirilmesi gibi
uygulamalarda da kullanılmaktadır.  
  
<!--more-->

Çoğu görüntü işleme uygulamasında renkli imge 24 bitten oluşmakta ve
yaklaşık 16 milyon renk barındırmaktadır. Gri seviye dönüşümü ile elde
edilen tek kanallı imgede ise 8 bit ile tutulan 256 farklı gri tonu
bulunmaktadır. Renk düzeyleri üzerinden bir hesaplama yapılırsa dönüşüm
sonucunda renkli imgedeki yaklaşık 65 bin (16 milyon / 256 ) rengin tek
bir siyahın tonu ile ifade edildiği görülür. Aşağıda verilen imgede yer
alan bütün renklerin gri seviye dönüşümü sonucu aldığı değerler yaklaşık
aynıdır.  
  
![Eş Parlaklık Seviyeleri][isoluminance]
  
Bu çok yüksek veri sıkıştırmasına imge renklendirme açısından bakacak
olursak, ortalamada tek bir siyahın tonuna karşılık olası 65 bin farklı
renk bulunduğu görülür. Tek bir gözek için 65 bin farklı olasılığın
bulunduğu bu problemde, akıllı bir yöntem kullanmadan 5x5 boyutunda bir
imge için dahi hesaplamaları yapmak oldukça güçtür.  
  
Yazımızın konusu olan imge renklendirme için, literatürdeki en ilgi
çekici yayın 2004 yılı Siggraph konferansında *Colorization using
Optimization* yayını ile önerilmiştir. Çalışmada renkli imgedeki
renkleri bulabilmek için imgenin bazı noktaları elle çizikler atarak
renklendirilmiştir. Ardından yapılan bu renklendirmeler iteratif olarak
gri seviyeler üzerine yayılarak imgenin tamamının renklenmesi
sağlanmıştır. Aşağıda bir imgenin renklendirilmesine ilişkin adımlar
gösterilmiştir.  
  
![Görüntü Renklendirme Adımları][steps]
  
Resimde ilk sırada renklendirmek istenen gri seviye imge yer almaktadır.
İkinci sırada ise algoritmanın başlayabilmesi için elle işaretlenen
kısmi renkli imge verilmiştir. Üçüncü sıradan itibaren de sırayla
algoritmanın 50., 250. ve 1000. iterasyonlarına ait sonuçlar
gösterilmiştir. Dikkatli bakılacak olursa elle işaretlenen renkler adım
adım gri seviyeler üzerine yayılmakta ve bu sırada imgedeki doku ve
parlaklık bilgisi de korunmaktadır.  
  
Yöntemin çalışması iki temel prensibe dayanmaktadır.  
  
**1)** *Renklendirme işlemi ile elde edilen renkli imgenin ( yukarıda
son sıradaki resim ) gri seviyesi, verilen gri imge ile birebir aynı
olmak zorundadır. *  
  
Bu koşulu sağlamak için RGB uzayı yerine bir kanalı gri seviyeyi
gösteren bir uzay (HSV, YUV, YIQ, vs.) kullanmak problemi oldukça
basitleştirmektedir. Böyle bir uzay dönüşümü sonrasında aranan renkli
imgenin gri seviye kanalı (HSV ise V, YUV ise Y kanalı) doğrudan giriş
resmine eşitlenebilir. Renklerin aranması ise diğer iki kanal (HSV ise H
ve S, YUV ise U ve V)  üzerinden yapılırsa, imgenin gri seviyesi
değişmeden renkleri değiştirilebilir. Yazımızda bildiride de önerildiği
gibi YUV renk uzayı kullanılmış ve dönüşüm için aşağıdaki kod satırları
kullanılmıştır.

  
```c
counter = 0;
for(int n=0; n < N; n++) 
{
    for(int m=0; m < M; m++) 
    {

        Y[counter] = 0.00392156*(0.299*G.pixels[m][n].red+0.587*G.pixels[m][n].green+0.114*G.pixels[m][n].blue);
        U[counter] = 0.00392156*(0.596*I.pixels[m][n].red-0.274*I.pixels[m][n].green-0.322*I.pixels[m][n].blue);
        V[counter] = 0.00392156*(0.211*I.pixels[m][n].red-0.523*I.pixels[m][n].green+0.312*I.pixels[m][n].blue);
        counter++;
    }
}
```
  
  
**2)** *Belirli bir komşulukta yer alan iki gri seviye birbirine
yakınsa, -renklendirme işlemi sonrası- renkleri de birbirine yakın
olmalıdır.*  

Bu  prensip ise aşağıdaki formülasyon ile bir enerji optimizasyonu
problemine dönüştürülebilir.

$$E(U)=\sum_{r} \left ( U(r)- \sum_{s\in N(r)} w_{rs}U(s) \right)^2$$

Yukarıda verilen ifade de $U$, YUV renk uzayının bir bileşenini
göstermektedir ve çalışmada bu ifade $V$ için de çözülmektedir. İfade,
bir $r$ gözeğinin değerinin, kendisini çevreleyen $N(r)$ komşuluğundaki
gözeklerin ağırlıklı ortalamasına yakın olmasını zorlamaktadır. Burada
$w_{rs}$, komşu gözeğin merkez gözeğe olan uzaklığını ölçmek için
kullanılan ağırlık metriğini göstermektedir ve $$w_{rs} \sim e^{-
(Y(r)-Y(s))^2}$$ formülü ile hesaplanmaktadır. Bu ifade basit şekilde
$r$ ve $s$ noktaları arasındaki gri seviye farklılığı ölçmektedir.
Çalışmada komşuluk derecesi bir olarak seçilmiş ve herhangi bir gözeğin
etrafını saran sekiz gözek komşu olarak değerlendirilmiştir. Aşağıda
verilen kod satırlarında imgenin gözekleri tek tek dolanılmış ve renksiz
olan her gözek için 8 komşusunun bu gözeğe olan gri seviye farklılıkları
hesaplanmıştır. Hesaplanan bu değerler $A$ değişkeninde saklanmıştır.

  
```c
for(r=0; r < M*N; r++) 
{
    if( (U[r]*U[r] + V[r]*V[r]) < 0.00001 ) 
    {
        tsum = 0;
        for(k=0; k < 8; k++) {
        D[k] = exp ( -0.5*(Y[k2c(r,k,M,N)]-Y[k2c(r,k,M,N)])*(Y[k2c(r,k,M,N)]-Y[k2c(r,k,M,N)]) );
        tsum += D[k];
        }
        for(k=0; k < 8; k++) { A[k][r] = -D[k]/tsum; }
    }
}
```
  

Optimizasyon problemini çözmeye geçmeden önce matrisleri kullanarak
enerji ifadesini daha sade ve çözümü daha kolay görülür şekilde yazalım.
Yukarıda verilen enerji ifadesi $$E(U)=||U(r)-W_{rs}U(s)||^2$$matrissel
biçiminde yazılabilir. Bu durumda enerji fonksiyonunu en küçükleyen
$U(s)$ vektörü de, bilinen $U(r)$ ve $W_{rs}$ matrisleri kullanılarak
$$U(s)=W_{rs}^{-1}U(r)$$işlemi ile bulunur. Burada $W_{rs}$ matrisi
her satırın da 8 komşu için ağırlıkları saklayan $MN \times MN$ boyutlu
bir seyrek matrisdir. $W_{rs}$ matrisini $3 \times 3$ boyutunda örnek
bir imge üzerinden kavrayalım. Aşağıdaki resimde imgede yer alan her bir
gözeğin (yeşil) komşuları (kırmızı) gösterilmiştir.  
  
![Görüntü Renklendirme Algoritması][neighbours]
  
Bu imgede yer alan gözekler için oluşturulan ağırlık matrisi $W_{rs}$
aşağıdaki şekilde olacaktır. Matriste birinci satır 1. gözeğin, ikinci
satır 2. gözeğin, dokuzuncu satır 9. gözeğin komşularına olan
uzaklıklarını barındıracaktır.  
  
\\begin{equation}  
\left \[  
\\begin{array}{c}  
U'(1)\\\\U'(2)\\\\U'(3)\\\\U'(4)\\\\U'(5)\\\\U'(6)\\\\U'(7)\\\\U'(8)\\\\U'(9)  
\\end{array}  
\right \] =  
\left \[  
\\begin{array}{ccccccccc}1 & w_{1}^{2} & 0 & w_{1}^{4} & w_{1}^{5} &
0 & 0 & 0 & 0\\\\w_{2}^{1} & 1 & w_{2}^{3} & w_{2}^{4} & w_{2}^{5} &
w_{2}^{6} & 0 & 0 & 0\\\\0 & w_{3}^{2} & 1 & 0 & w_{3}^{5} &
w_{3}^{6} & 0 & 0 & 0\\\\w_{4}^{1} & w_{4}^{2} & 0 & 1 & w_{4}^{5} &
0 & w_{4}^{7} & w_{4}^{8} & 0\\\\w_{5}^{1} & w_{5}^{2} & w_{5}^{3}
& w_{5}^{4} & 1 & w_{5}^{6} & w_{5}^{7} & w_{5}^{8} &
w_{5}^{9}\\\\0 & w_{6}^{2} & w_{6}^{3} & 0 & w_{6}^{5} & 1 & 0 &
w_{6}^{8} & w_{6}^{9}\\\\0 & 0 & 0 & w_{7}^{4} & w_{7}^{5} & 0 & 1 &
w_{7}^{8} & 0\\\\0 & 0 & 0 & w_{8}^{4} & w_{8}^{5} & w_{8}^{6} &
w_{8}^{7} & 1 & w_{8}^{9}\\\\0 & 0 & 0 & 0 & w_{9}^{5} & w_{9}^{6} &
0 & w_{9}^{8} & 1\\\\\\end{array}  
\right \]^{-1}  
\left \[  
\\begin{array}{c}  
U(1)\\\\U(2)\\\\U(3)\\\\U(4)\\\\U(5)\\\\U(6)\\\\U(7)\\\\U(8)\\\\U(9)  
\\end{array}  
\right \]  
\\end{equation}  
  
Yukarıdaki formulasyonda da gösterildiği üzere yapmamız gereken işlemde
bu matrisin tersine ihtiyacımız vardır. Matrisin boyutu ($MN \times
MN$) resmin boyutunun ($MN$) karesi ile hesaplandığından işlem yükü
oldukça ağır sanılabilir ancak matrisin her satırında en fazla 8 eleman
0 dan farklı olduğundan uygun algoritmalarla tersi bulma işlemi oldukça
hızlandırılabilir. Bu çalışmada Gauss-Seidel yöntemi kullanılarak $U$ ve
$V$ kanalları bulunmuştur.  
  
### Gauss-Seidel

Gauss-Seidel yöntemi $Ax=b$ şeklinde verilen bir doğrusal denklem
takımını iteratif olarak çözmekte kullanılan bir yöntemdir. $A$,
$N\times N$ matris, $x$ ve $b$ $N\times 1$ vektörler olmak üzere
aranan $x$ vektörü şu iteratif adımlarla bulunur.
$$x_{i}^{k+1}=\frac{1}{a_{ii}} \left ( b_i - \sum_{j=i}^{i-1}
a_{ij}x_{j}^{k+1} - \sum_{j=i+1}^{N} a_{ij}x_{j}^{k} \right )$$
Denklem de verilen $A$ matrisi imge renklendirme probleminde ağırlık
matrisini gösterdiğinden ve çok az sayıda elemanı 0 dan farklı
olduğundan, uygun kod yazılması durumunda çözüm için gerekli işlem
sayısı oldukça az olacaktır. Aşağıda verilen kod satırlarında $A$ ve $b$
matrisleri kullanılarak $x$ vektörü iteratif şekilde bulunur.

  
```c
void iterative_solver(double *A[9], double *b, int M, int N, int S) 
{
    double sum;
    int k,t,s, Max_Iter = 500;

    double *x  = (double*) calloc(S,sizeof(double));
    memcpy(x, b, S*sizeof(double));

    for(t=0; t < Max_Iter; t++) 
    {
        for(s=0; s < M*N; s++) 
        {
            sum = 0;
            for(k=0; k < 8; k++) {
                sum += A[k][s]*x[k2c(s,k,M,N)];
            }
            x[s] = (b[s]-sum);
        }
    }
    memcpy(b, x, S*sizeof(double));
    free(x);
}
```
  
Yukarıda verilen kod $U$ ve $V$ kanalları için çalıştırılıp sonuçlar
bulunduktan sonra, renkli resim YUV uzayından RGB uzayına çevrilerek
elde edilir.  
  
Çalışmanın sonuçlarına ait örnekler ve kaynaklar aşağıda paylaşılmıştır.
Bu paylaşımda anlatımı ve kodlamayı kolaylaştırmak için orjinal yayından
kısmen farklı formüller ve algoritmalar kullanılmıştır. Bu nedenle
aşağıdaki örnek renklendirmelerde orjinal algoritmadan biraz farklı ama
kabul edilebilir sonuçlar elde edilmiştir. Görüntülerde ilk resimler gri
kaynağı, ikinci resimler renklendirme işlemi için yapılan
işaretlemeleri, üçüncü imge ise yazımızda anlatılan algoritmanın
sonucunu göstermektedir. Son sıradaki imge ise elde edilmeye çalışılan
renkli imgedir.  
  
![Görüntü Renklendirme Örnek][example1]
![Görüntü Renklendirme Örnek][example2]
  
**Referanslar**  
* Levin, Anat, Dani Lischinski, and Yair Weiss. ["Colorization using optimization."](http://kucg.korea.ac.kr/new/course/2005/CSCE352/paper/levin04.pdf) ACM transactions on graphics (tog). Vol. 23. No. 3. ACM, 2004.
*  Gauss-Seidel method wikipedia article. ["https://www.wikiwand.com/en/Gauss–Seidel_method"](https://www.wikiwand.com/en/Gauss–Seidel_method)

[RESOURCES]: # (List of the resources used by the blog post)
[isoluminance]: /assets/post_resources/image_colorization/iso_lum.png
[steps]: /assets/post_resources/image_colorization/tum_komsular.png
[example1]: /assets/post_resources/image_colorization/gray2color_example1.png
[example2]: /assets/post_resources/image_colorization/gray2color_example2.png