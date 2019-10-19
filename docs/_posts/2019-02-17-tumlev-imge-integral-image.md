---
layout: post
title: Tümlev İmge (Integral Image)
date: '2019-02-17T22:27:00.000+03:00'
author: Bahri ABACI
tags:
- Görüntü işleme
- Hızlı algoritmalar
modified_time: '2019-07-14T23:08:12.843+03:00'
thumbnail: https://1.bp.blogspot.com/-a8Qb8UGnqwc/XGK5wk_LzoI/AAAAAAAABuk/IdAUpAQATfIBxmg1eExPhJwe3WoygGw4ACLcBGAs/s72-c/integral_image_heading_200.png
blogger_id: tag:blogger.com,1999:blog-680077327709981593.post-8383829928164958944
blogger_orig_url: http://www.cescript.com/2019/02/tumlev-imge-integral-image.html
content_markdown: 'content/integral_image/README.md'
---

{% capture post_content %}{% include_relative {{ page.content_markdown |  }} %}{% endcapture %}

{% assign post_content_split = post_content | split: site.excerpt_separator %}
{{post_content_split[0] | markdownify}}
<!--more-->
{{post_content_split[1] | markdownify}}