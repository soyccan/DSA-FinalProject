## Send / Recieve Email 

The user need to turn on **Access to less secure apps** and **IMAP** of his/her google account to enable these two features.

#### Main Functions:\
++Send Mail++: 
```python
def send_msg(account, password, dir_path):
```
1. Login gmail with `account` and `password`
2. Receive user input on GUI and contstruct message by `construct_msg()`
3. Send the mail and let smtp.gmail.com to forward mails
4. Save a mail file under `dir_path`  formatted by the spec of this homework. 

++Load mails from gmail++: 
```python
def read_gmail(account, password, dir_path, startID):
```

1. Login gmail with `account` and `password`
2. Receive user input on GUI to determine how many  mails to be load, said $N$. 
4. Load $N$ mails from `account` to local client and formate them by the spec of this homework
5. Save the loaded mail file under `dir_path`

#### Restriction \
The format of email that we are able to send is restricted. It can only contains following items: 

- From: the account of user's gmail
- To: a valid \<mail address\>
- Subject: \<strings\>
- Content: \<strings\>, cannot contain any attachment file 

#### Name Pipe for Communication \
Our shell script creates four name pipes to forward the signals for communication of python script and c++ GUI.

- Python -> `/tmp/py_out_pipe` -> `/tmp/qt_in_pipe` -> C++ GUI 
- C++ GUI -> `/tmp/qt_out_pipe` -> `/tmp/py_in_pipe` -> Python

## GUI
We use Qt creator to devolop a GUI which can actually send email(ex: gmail) and achieve the functions the homework required.
We use C++ languange base on OOP to write code.

#### Features \
++Login Interface++ \
<img src="https://i.imgur.com/xhxGZQB.png" style="width:200px; float:right">
When the user run this program, the first thing they need to do is login to the gmail.
The accountInputBox and passwordInputBox objects will get the words user entered.\
When the user click login button, the program will communicate to the python program above and check if it is login successfully. \
If the user log in successfully, they will go into main menu page. \
Otherwise, a message box will jump out and tell you "login failed". \
When th user click exit button, the program will just closed.

++Main Menu Interface++ \
<img src="https://i.imgur.com/L27yslx.png" style="width:200px; float:right">
If the user login successfully, they will go into this page. \
When the user clicked reload button, the program will call the python function and load gmail into database. \
When the user clicked longest button, they will see the mail with longest content. If there isn't any mail in the data base, the ID label and length label will show "-1". \
After the user login, the only way to close this program gracefully is to click the exit button, and the button is only appears here.
Namely, they must back to the main page if they want to stop the program. \
If the user click other buttons, they will go into the interfaces below.

++Send Email Interface++ \
<img src="https://i.imgur.com/fzbFXtK.png" style="width:200px; float:right">
When the user click send email button, they will go into this page. \
The user must to type something in the label of "To, Subject and Content" so that they can send email. \
When the user click the send button, the program will do: \
(1) Check if they do type something in the label of "To, Subject and Content", or else they will see a message box jump out and says "Must fill in to, subject and content". \
(2) Call the python code above. If it fail to send the email, they will
see the message box contains "cannot send email".

++Add Email to Database Interface++ \
<img src="https://i.imgur.com/fYAMoye.png" style="width:200px; float:right">
When the user click add email button, they will go into this page. \
The difference between add email and send email is that:
Send email really send gmail to the people you want to sent, and they will see a mail when they open their gmail page. \
Add email just add an email which you load from reload function in the menu page to the datbase.
When the user click the add button, they will see how many number of mails are in the database if they add successfully. \
Otherwise, they will see "-1" in the mail num.

++Search Email with keyword Interface++
<img src="https://i.imgur.com/H3AZ1e5.png" style="width:200px; float:right">
When the user click query button, they will go into this page. \
When the user clicked search button, the program will check if the user fill in the expression field. If not, they will see "type message" in the message box. \
If they did not find anything,they will just show nothing in the result id field.


