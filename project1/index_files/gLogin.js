// var googleUser = {};

var config = {
	apiKey: "AIzaSyBWvRG_66XFJGwLXUKhTkVIlLr_GU3bTfI",
	authDomain: "alpawebapplication.firebaseapp.com",
	databaseURL: "https://alpawebapplication.firebaseio.com",
	projectId: "alpawebapplication",
	storageBucket: "",
	messagingSenderId: "275450919441"
};
firebase.initializeApp(config);


var user = firebase.auth().currentUser;

var no_login = document.getElementsByClassName('no_login');
console.log(no_login[0]);

var url = {};
for ( var i = 0; i < no_login.length; i++) {
	url[i] = no_login[i].attributes[0].nodeValue;
}
console.log(url);

if (user) {

  // User is signed in.
} else {
	for ( var i = 0; i < no_login.length; i++) {
		console.log(no_login[i]);

		no_login[i].attributes[0].nodeValue = '/';
	}	// console.log($('.no_login'));
	// no_login[0].removeAttribute('href');
	// no_login[0].setAttribute('href','/');
  // No user is signed in.
}
// var startApp = function() {
//     gapi.load('auth2', function(){

//       auth2 = gapi.auth2.init({
//         client_id: '275450919441-sbr4ri09uabnfjsic949sjir534119n9.apps.googleusercontent.com',
//         cookiepolicy: 'flHX2LHhHoAV0ZXcJ3aB6spN',

//       });
//       attachSignin(document.getElementById('customBtn'));
//     });
//   };

// function attachSignin(element) {
//  	console.log(element.id);
//  	auth2.attachClickHandler(element, {},
//  		function(googleUser) {
//  		   var profile = googleUser.getBasicProfile();
//  		   $("#gSignInWrapper").css("display","none");
//  		   $(".data").css("display","block");
//  		   $("#pic").attr('src',profile.getImageUrl());
//  		   $("#email").text(profile.getEmail());
//  		}, function(error) {
//  			alert(JSON.stringify(error, undefined, 2));
//  		});
// }

// function signOut() {
// 	var auth2 = gapi.auth2.getAuthInstance();
// 	auth2.signOut().then(function(){

// 		alert("로그 아웃!");

// 		$("#gSignInWrapper").css("display","block");
//  		$(".data").css("display","none");
// 	})
// }

function _signIn() {
	var provider = new firebase.auth.GoogleAuthProvider();
	firebase.auth().signInWithPopup(provider).then(function (result) {
		// This gives you a Google Access Token. You can use it to access the Google API.
		var token = result.credential.accessToken;
		// The signed-in user info.\

		console.log(1);
		var user = result.user;
		console.log(user);
		document.getElementById('btnText').textContent = user.displayName;
		//////////////////////////////////////////////////////////////////////

		document.getElementById('email').setAttribute('value', user.email);

				document.getElementById('email2').setAttribute('value', user.email);
						document.getElementById('email3').setAttribute('value', user.email);
								document.getElementById('email4').setAttribute('value', user.email);
								document.getElementById('NoticeEmail').setAttribute('value', user.email);
								document.getElementById('SeminarEmail').setAttribute('value', user.email);



		///////////////////////////////////////////////////////////////////////s
		for ( var i = 0; i < no_login.length; i++) {
			no_login[i].attributes[0].nodeValue = url[i];
			console.log(no_login[i]);

		}
		// ...
	}).catch(function (error) {
		// Handle Errors here.
		var errorCode = error.code;
		var errorMessage = error.message;
		// The email of the user's account used.
		var email = error.email;
		// The firebase.auth.AuthCredential type that was used.
		var credential = error.credential;

		// ...
	});

	var user = firebase.auth().currentUser;

	if (user) {
		console.log(1);

		// User is signed in.
	} else {
		console.log(2);

		// No user is signed in.
	}
}

function signIn() {
	firebase.auth().setPersistence(firebase.auth.Auth.Persistence.LOCAL)
			.then(function () {

				return _signIn();
			})
			.catch(function (error) {
				// Handle Errors here.
				var errorCode = error.code;
				var errorMessage = error.message;
			});
}

// document.getElementById('customBtn').addEventListener('click', function () {
// 	firebase.auth().setPersistence(firebase.auth.Auth.Persistence.NONE)
// 		.then(function () {
// 			return signIn();
// 		})
// 		.catch(function (error) {
// 			// Handle Errors here.
// 			var errorCode = error.code;
// 			var errorMessage = error.message;
// 		});

	// firebase.auth().setPersistence(firebase.auth.Auth.Persistence.LOCAL)
	// .then(function() {
	//   // Existing and future Auth states are now persisted in the current
	//   // session only. Closing the window would clear any existing state even
	//   // if a user forgets to sign out.
	//   // ...
	//   // New sign-in will be persisted with session persistence.
	//   return signIn();
	// })
	// .catch(function(error) {
	//   // Handle Errors here.
	//   var errorCode = error.code;
	//   var errorMessage = error.message;
	// });

// });



function signOut() {
	firebase.auth().signOut().then(function () {
		// Sign-out successful.
	}).catch(function (error) {
		// An error happened.
	});
}
