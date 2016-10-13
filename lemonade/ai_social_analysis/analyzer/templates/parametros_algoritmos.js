DEFAULT_PARAMS = {{ default|safe }};

$('#id_algorithm').change( function() {{
    $('#id_algorithm_params').val(DEFAULT_PARAMS[this.value])  
}});
