from django.contrib import admin

from .models import Dataset, Algorithm, Task

admin.site.register(Dataset)
admin.site.register(Algorithm)
admin.site.register(Task)

