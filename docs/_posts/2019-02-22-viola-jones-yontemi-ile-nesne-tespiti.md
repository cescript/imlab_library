---
layout: post
title: Viola-Jones Yöntemi ile Nesne Tespiti (Object Detection)
date: '2019-02-22T22:48:00.000+03:00'
author: Bahri ABACI
tags:
- Görüntü işleme
- Nesne Tespiti
modified_time: '2019-07-14T23:06:47.017+03:00'
thumbnail: https://2.bp.blogspot.com/-Mlq3cLxCx08/XF2SzB7Xl3I/AAAAAAAABtw/13ydpfvn1Iw2QR3NPD-TRb-AbGjdOT_VQCLcBGAs/s72-c/detected_objects.png
blogger_id: tag:blogger.com,1999:blog-680077327709981593.post-7533078498951748386
blogger_orig_url: http://www.cescript.com/2019/02/viola-jones-yontemi-ile-nesne-tespiti.html
content_markdown: 'content/object_detection/README.md'
---

{% capture post_content %}{% include_relative {{ page.content_markdown |  }} %}{% endcapture %}

{% assign post_content_split = post_content | split: site.excerpt_separator %}
{{post_content_split[0] | markdownify}}
<!--more-->
{{post_content_split[1] | markdownify}}