---
layout: post
title: IMLAB Görüntü İşleme Kütüphanesi
date: '2018-12-03T19:41:00.000+03:00'
author: Bahri ABACI
tags:
- Görüntü işleme
modified_time: '2019-07-14T23:08:49.580+03:00'
thumbnail: https://2.bp.blogspot.com/-mBfDLx_V1nQ/XAQeVHN2MhI/AAAAAAAABs4/2iL5_AxebiQF-aXAeFcDDreuoJOcmFxxQCEwYBhgL/s72-c/cescrip_imlab.png
blogger_id: tag:blogger.com,1999:blog-680077327709981593.post-3063006083148120967
blogger_orig_url: http://www.cescript.com/2018/12/imlab-goruntu-isleme-kutuphanesi.html
content_markdown: 'content/imlab_library/README.md'
---

{% capture post_content %}{% include_relative {{ page.content_markdown |  }} %}{% endcapture %}

{% assign post_content_split = post_content | split: site.excerpt_separator %}
{{post_content_split[0] | markdownify}}
<!--more-->
{{post_content_split[1] | markdownify}}