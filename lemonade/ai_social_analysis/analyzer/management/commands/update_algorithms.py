import datetime
import json

from django.core.management.base import BaseCommand

from analyzer.models import Algorithm
from analyzer import algorithms


class Command(BaseCommand):

    help = 'Check for new algorithms and update the database'

    def handle(self, *args, **options):
        
        module_names = [name for name in dir(algorithms) if (not name.startswith("__")) and (name != "utils")]

        for module_name in module_names:

            module = getattr(algorithms, module_name)

            updated_values = {
                "name"           : module.name,
                "module_name"    : module_name,
                "params_default" : json.dumps(module.params_default, indent=4)
            }

            obj, created = Algorithm.objects.update_or_create(module_name=module_name, defaults=updated_values)
            print("- Algorithm: {} \"{}\" --> {}".format(module_name, module.name, created))

