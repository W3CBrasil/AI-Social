Rails.application.routes.draw do
  root to: 'home#index'
  resources :dataflows
  resources :templates
  resources :interfaces
#  get 'dataflows/modal/:id' => 'dataflows#modal', as: :modal
  get "/home/index" => "home#index"
  get 'dataflows/get_interface/:id' => 'dataflows#get_interface', as: :get_interface
  match 'dataflows/execute/:id' => 'dataflows#execute_dataflow', as: :execute, via: [:get, :post]
  get 'dataflows/stop/:id' => 'dataflows#stop_dataflow', as: :stop
  get "/services/:service" => "services#show"
  post "/dataflows/update_run" => "dataflows#update_run", :as => :update_run
  get 'locale/:id' => 'locales#set_locale', as: :set_locale

  Rails.application.routes.draw do
    devise_for :users, controllers: {
      sessions: 'users/sessions'
    }
  end
end

