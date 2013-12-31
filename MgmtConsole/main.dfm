object mainForm: TmainForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'Management Console'
  ClientHeight = 591
  ClientWidth = 788
  Color = clBtnFace
  Constraints.MaxHeight = 618
  Constraints.MaxWidth = 796
  Constraints.MinHeight = 618
  Constraints.MinWidth = 796
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  ScreenSnap = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 505
    Height = 564
    Caption = 'Users'
    TabOrder = 0
    object Label3: TLabel
      Left = 8
      Top = 532
      Width = 94
      Height = 13
      Caption = 'Data Cycle per sec:'
    end
    object Label4: TLabel
      Left = 106
      Top = 532
      Width = 6
      Height = 13
      Caption = '0'
    end
    object Button1: TButton
      Left = 400
      Top = 528
      Width = 97
      Height = 25
      Caption = 'New'
      Enabled = False
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 297
      Top = 528
      Width = 97
      Height = 25
      Caption = 'Delete'
      Enabled = False
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 194
      Top = 528
      Width = 97
      Height = 25
      Caption = 'Edit'
      Enabled = False
      TabOrder = 2
      OnClick = Button3Click
    end
  end
  object GroupBox2: TGroupBox
    Left = 519
    Top = 8
    Width = 253
    Height = 146
    Caption = 'Connection'
    TabOrder = 1
    object Label1: TLabel
      Left = 11
      Top = 32
      Width = 24
      Height = 13
      Caption = 'Port:'
    end
    object Label2: TLabel
      Left = 11
      Top = 59
      Width = 50
      Height = 13
      Caption = 'Password:'
    end
    object ComboBox1: TComboBox
      Left = 39
      Top = 29
      Width = 202
      Height = 21
      TabOrder = 0
    end
    object Edit1: TEdit
      Left = 67
      Top = 56
      Width = 174
      Height = 21
      PasswordChar = '*'
      TabOrder = 1
    end
    object Button4: TButton
      Left = 19
      Top = 83
      Width = 63
      Height = 25
      Caption = 'Connect'
      TabOrder = 2
      OnMouseDown = Button4MouseDown
    end
    object Button5: TButton
      Left = 88
      Top = 83
      Width = 74
      Height = 25
      Caption = 'Disconnect'
      Enabled = False
      TabOrder = 3
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 168
      Top = 83
      Width = 66
      Height = 25
      Caption = 'Fire Info'
      Enabled = False
      TabOrder = 4
      OnClick = Button6Click
    end
    object Button7: TButton
      Left = 88
      Top = 114
      Width = 74
      Height = 25
      Caption = 'Download'
      Enabled = False
      TabOrder = 5
      OnClick = Button7Click
    end
    object Button8: TButton
      Left = 168
      Top = 114
      Width = 65
      Height = 25
      Caption = 'Set Pass'
      TabOrder = 6
      Visible = False
    end
    object Button9: TButton
      Left = 19
      Top = 114
      Width = 63
      Height = 25
      Caption = 'Clear'
      TabOrder = 7
      OnClick = Button9Click
    end
  end
  object ListView1: TListView
    Left = 16
    Top = 24
    Width = 489
    Height = 506
    Columns = <
      item
        Caption = 'Id'
        Width = 40
      end
      item
        Caption = 'Name'
        Width = 120
      end
      item
        Caption = 'Data'
        Width = 150
      end
      item
        Caption = 'Counter'
        Width = 80
      end
      item
        Caption = 'Allowed'
        Width = 80
      end>
    HideSelection = False
    ReadOnly = True
    RowSelect = True
    TabOrder = 2
    ViewStyle = vsReport
  end
  object Memo1: TMemo
    Left = 519
    Top = 160
    Width = 253
    Height = 386
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 3
  end
  object ProgressBar1: TProgressBar
    Left = 519
    Top = 552
    Width = 253
    Height = 20
    Max = 200
    Step = 2
    TabOrder = 4
  end
  object Timer2: TTimer
    OnTimer = Timer2Timer
    Left = 80
    Top = 464
  end
  object oneTimeTimer: TTimer
    Enabled = False
    Interval = 50
    Left = 80
    Top = 424
  end
  object oneTimeTimer2: TTimer
    Enabled = False
    Interval = 50
    OnTimer = oneTimeTimer2Timer
    Left = 81
    Top = 385
  end
  object ActionList1: TActionList
    Left = 312
    Top = 304
    object Action1: TAction
      Caption = 'Action1'
    end
    object Action2: TAction
      Caption = 'Action2'
    end
    object Action3: TAction
      Caption = 'Action3'
    end
  end
  object PABar1: TPopupActionBar
    Left = 400
    Top = 328
    object A1: TMenuItem
      Caption = 'As Manager'
      OnClick = A1Click
    end
    object AsConsole1: TMenuItem
      Caption = 'As Console'
      OnClick = AsConsole1Click
    end
  end
end
