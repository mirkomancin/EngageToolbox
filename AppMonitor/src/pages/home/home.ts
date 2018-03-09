import { SettingsPage } from './../settings/settings';
import { Component } from '@angular/core';
import { NavController, Platform } from 'ionic-angular';
import { HTTP } from '@ionic-native/http';
import { Storage } from '@ionic/storage';
import {TranslateService} from "@ngx-translate/core";

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {

  consumo = 0;
  produzione = 0;
  guadagno = 0;
  TOKEN = "";
  refreshTime = 5000;

  constructor(
    public navCtrl: NavController,
    private http: HTTP,
    private platform: Platform,
    public storage: Storage,
    private translate : TranslateService) {
      
  }

  ionViewDidEnter(){
    let _self = this;

    this.storage.get('refreshTime')
    .then((val) => {
      if(val && parseInt(val)>0){
        _self.refreshTime = parseInt(val)*1000;

        _self.storage.get('token')
          .then((val) => {
            console.log('Your token is', val);
            if(val.length>1){
              _self.TOKEN = val;
              _self.retrieveData();
              setInterval(function(){
                _self.retrieveData();
              },_self.refreshTime);
            }
            else{
              console.log('TOKEN NULL');
              _self.navCtrl.push(SettingsPage);
            }
          })
          .catch((val) => {
            console.log('TOKEN INVALID');
            _self.navCtrl.push(SettingsPage);
          });

      }
      else{
        console.log('REFRESH TIME NULL');
        _self.navCtrl.push(SettingsPage);
      }
    })
    .catch((val) => {
      console.log('REFRESH TIME NOT INVALID');
      _self.navCtrl.push(SettingsPage);
    });
       
  }

  retrieveData(){
    let _self = this;
    this.http.get('https://engage.efergy.com/mobile_proxy/getCurrentValuesSummary?token='+this.TOKEN, {}, {})
      .then(data => {

        console.log(data.status);
        console.log(data.data); // data received by server
        console.log(data.headers);

        let json = JSON.parse(data.data);
        for(var i=0; i<json.length; i++){
					if(json[i]["cid"]=="PWER"){
						_self.consumo = json[i]["data"][0][Object.keys(json[i]["data"][0])[0]];
					}
					if(json[i]["cid"]=="PWER_GAC"){
						_self.produzione = json[i]["data"][0][Object.keys(json[i]["data"][0])[0]];
					}
				  }

				  //_self.produzione-=65;
				  
				  _self.guadagno = _self.produzione-_self.consumo;
				  _self.guadagno = Math.floor(_self.guadagno*100)/100;
      })
      .catch(error => {

        console.log(error.status);
        console.log(error.error); // error message as string
        console.log(error.headers);

      });
  }

  exitApp(){
    this.platform.exitApp();
  }

}
