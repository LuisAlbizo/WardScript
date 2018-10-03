
i := 5;

f := func x: g:
	g := func y: y:
		present(x);
		present(var);
		y := x + y;
	end;
end;

c := func x: g:
	var := 90;
	g := f(x);
end;

g := c(7);
present(g(14));

