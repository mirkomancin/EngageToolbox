import { Component } from '@angular/core';
import { NavController, NavParams } from 'ionic-angular';
import { Storage } from '@ionic/storage';

@Component({
  selector: 'page-settings',
  templateUrl: 'settings.html'
})
export class SettingsPage {
  
  TOKEN = "";
  token_valid = false;
  refreshTime = 5;

  constructor(
    public navCtrl: NavController, 
    public navParams: NavParams,
    public storage: Storage) {
    
  }

  ionViewWillEnter(){
    let _self = this;
    this.storage.get('token')
    .then((val) => {
      console.log('Your token is', val);
      _self.TOKEN = val;
      _self.token_valid = true;
    })
    .catch((val) => {
      console.log('TOKEN INVALID!!');
    });

    this.storage.get('refreshTime')
    .then((val) => {
      if(val){
        console.log('Refresh time: ', val);
        _self.refreshTime = val;
      }
    })
    .catch((val) => {
      console.log('Refresh time not valid!');
    });
  }

  onBtnSave(){
    console.log("SAVE");
    this.storage.set('token', this.TOKEN)
      .then(function(a){
        console.log("OK",a);
      })
      .catch(function(err){
        console.log("ERR",err);
      });

    this.storage.set('refreshTime', this.refreshTime)
      .then(function(a){
        console.log("OK",a);
      })
      .catch(function(err){
        console.log("ERR",err);
      });
  }

}
