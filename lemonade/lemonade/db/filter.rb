# Arquivo responsavel por popular template filter no banco

#
#
def delete
  Template.delete_all(category: 'filter')
  Interface.delete_all(category: 'filter')
end



def pop_templates templates
  templates.each do |value|
    Template.create(value)
  end
end

def pop_interfaces interface
  interface.each do |value|
    Interface.create(value)
  end
end

def pop_filter

  templates = []
  interfaces = []

  #CSV filter
  templates <<  { name:'replace_string',
                  display_name:'Replace',
                  category: 'filter',
                  parameters: {
                    text: { "Step name": 'replace',search: 'word', replace: 'word'},
                    select: {},
                    check_box: {},
                    browser: {}
                  }
  }
  interfaces << { name:'replace_string',
                 display_name: 'Replace',
                 position: { x: 350, y: 300},
                 size: { width: 80, height: 80 },
                 category: 'filter',
                 inPorts: [''],
                 outPorts: [''],
                 attrs: {
                   '.body': { stroke: '#ff471a'},
                   '.label': {text: 'Replace', fill:'#ff471a'},
                   '.port-body': { fill: '#ff8566'}}
  }
#  delete
  pop_templates templates
  pop_interfaces interfaces
end
