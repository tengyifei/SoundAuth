object newUserDlg: TnewUserDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'New User...'
  ClientHeight = 146
  ClientWidth = 398
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 76
    Height = 21
    AutoSize = False
    Caption = 'Name:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 35
    Width = 76
    Height = 21
    AutoSize = False
    Caption = 'Auth Info:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 8
    Top = 62
    Width = 76
    Height = 21
    AutoSize = False
    Caption = 'Counter:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label4: TLabel
    Left = 8
    Top = 89
    Width = 76
    Height = 21
    AutoSize = False
    Caption = 'Id Info:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Button1: TButton
    Left = 234
    Top = 116
    Width = 75
    Height = 25
    Caption = 'Confirm'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 315
    Top = 116
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Edit1: TEdit
    Left = 90
    Top = 8
    Width = 300
    Height = 21
    TabOrder = 2
  end
  object Edit2: TEdit
    Left = 90
    Top = 35
    Width = 300
    Height = 21
    TabOrder = 3
  end
  object Edit3: TEdit
    Left = 90
    Top = 62
    Width = 300
    Height = 21
    TabOrder = 4
  end
  object Edit4: TEdit
    Left = 90
    Top = 89
    Width = 300
    Height = 21
    TabOrder = 5
  end
end
