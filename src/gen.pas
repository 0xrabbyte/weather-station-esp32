
Var 
  a: ansistring;
Begin
  assign(input,'Microsoft IT SSL SHA2 - Baltimore');
  assign(output,'out');
  reset(input);
  rewrite(output);
  While Not(EOF) Do
    Begin
      readln(a);
      writeln('"',a,'\n" \');
    End;
  close(input);
  close(output);
End.
