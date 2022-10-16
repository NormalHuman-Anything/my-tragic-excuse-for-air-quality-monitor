from typing import Union


import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

cred = credentials.Certificate("./testserv-bdb7c-firebase-adminsdk-xnxl2-fcf790e2e6.json")
firebase_admin.initialize_app(cred, {
    'databaseURL' : 'https://testserv-bdb7c-default-rtdb.firebaseio.com/'
})


from fastapi import FastAPI, Request
from fastapi.responses import HTMLResponse
from fastapi.templating import Jinja2Templates
app = FastAPI()

templates = Jinja2Templates(directory="templates")

@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.get("/items/{item_id}")
def read_item(item_id: int, q: Union[str, None] = None):
    return {"item_id": item_id, "q": q}


@app.get("/name/{name}/age/{age}", response_class=HTMLResponse)
def read_name(request: Request, name: str, age: int):
    ref = db.reference(f"/{name}")
    ref.set(f"{age}")
    return templates.TemplateResponse("index.html", {"request":request, "name": name, "age": age})

    #GET과 POST request, 왜 GET을 사용했는지? -> 아두이노로는 POST보다 GET이 편하기 때문.

@app.get("/view/{name}", response_class=HTMLResponse)
async def read_value(request: Request, name: str):
  ref= db.reference(f"{name}")
  value= ref.get()
  print(value)
  return templates.TemplateResponse("index.html", {"request": request, "name": name, "age": value})



"""
<script type="module">
  // Import the functions you need from the SDKs you need
  import { initializeApp } from "https://www.gstatic.com/firebasejs/9.12.1/firebase-app.js";
  // TODO: Add SDKs for Firebase products that you want to use
  // https://firebase.google.com/docs/web/setup#available-libraries

  // Your web app's Firebase configuration
  const firebaseConfig = {
    apiKey: "AIzaSyBOQpY3GOpbNv5F3XRFyZsyf0PfXaWn8FY",
    authDomain: "testserv-bdb7c.firebaseapp.com",
    databaseURL: "https://testserv-bdb7c-default-rtdb.firebaseio.com",
    projectId: "testserv-bdb7c",
    storageBucket: "testserv-bdb7c.appspot.com",
    messagingSenderId: "563240891190",
    appId: "1:563240891190:web:ac5c1f9fedb1d75d143997"
  };

  // Initialize Firebase
  const app = initializeApp(firebaseConfig);
</script>
"""