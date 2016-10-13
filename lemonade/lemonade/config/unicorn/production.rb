working_directory "/home/ubuntu/lemonade/current"
worker_processes 2
listen "/tmp/lemonade.sock", backlog: 64
timeout 60
pid "/home/ubuntu/lemonade/shared/tmp/pids/unicorn.pid"
stderr_path "log/unicorn.stderr.log"
stdout_path "log/unicorn.stdout.log"

before_fork do |server, worker|
  defined?(ActiveRecord::Base) and ActiveRecord::Base.connection.disconnect!
end

after_fork do |server, worker|
  defined?(ActiveRecord::Base) and ActiveRecord::Base.establish_connection
end
